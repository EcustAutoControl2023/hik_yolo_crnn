/***************************************************************************
 * note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right
 *Reserved.
 *
 * @file         demo_alg.c
 * @brief        encapsulate four functions with the hikflow library
 *
 * @author       guanjiajun
 *
 * @date         2019/2/18
 * @version      1.0.0
 * @note         add functions for hikflow
 *
 * @date         2019/6/26
 * @version      2.0.0
 * @note         add english notes
 *
 * @date         2021/2/25
 * @version      3.0.0
 * @author       lixiaogang10
 * @note         Unified platform AND parse bin info
 *****************************************************************************/

// #include <cstring>
#include <stdio.h>
#include <sys/time.h>

#include "custom_callback.h"
#include "demo_alg.h"
#include "demo_comm.h"
#include "opdevsdk_hikflow_dev_man.h"
#include "opdevsdk_hka_types.h"
#include "opdevsdk_mem.h"

#define _CRT_SECURE_NO_WARNINGS
// #include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using namespace cv;

// #include "opdevsdk_hikflow_custom.h"
// #include "opencv2/opencv.hpp"

void *g_model_handle = NULL;
void *g_net_handle = NULL;
void *g_model_buffer = NULL;
unsigned long long g_model_phy_base = 0;

#define OPDEVSDK_CHAR 40
#define OPDEVSDK_MODEL_INFOLEN 256
#define OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG -1
#define OPDEVSDK_HIKFLOW_SIZE_ALIGN(size, align)                               \
  (((size) + ((align) - 1)) & (~((align) - 1)))
#define OPDEVSDK_HIKFLOW_SIZE_ALIGN_16(size)                                   \
  OPDEVSDK_HIKFLOW_SIZE_ALIGN(size, 16)
#define G5_PAGE_ALIGN (0x1000)

char g_pVersion[OPDEVSDK_CHAR] = {0};
CONFIG_DATA_T g_config_data = {0};
OPDEVSDK_HIKFLOW_PARAM_ST g_param_info_net = {0};
OPDEVSDK_HKA_MEM_TAB_ST g_hikflow_mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM] = {0};
OPDEVSDK_HKA_MEM_TAB_ST g_hikflow_mem_tab_model[OPDEVSDK_HKA_MEM_TAB_NUM] = {0};

///< check error
#define USE_DEBUG
#ifdef USE_DEBUG
#define OPDEVSDK_HIKFLOW_CHECK_ERROR(sts, str, ret)                            \
  {                                                                            \
    if (sts) {                                                                 \
      DEMOPRT((char *)"File: %s, Line: %05d: %s\n", __FILE__, __LINE__,        \
              (str));                                                          \
      return (ret);                                                            \
    }                                                                          \
  }
#else
#define OPDEVSDK_HIKFLOW_CHECK_ERROR(sts, str, ret)                            \
  {                                                                            \
    if (sts) {                                                                 \
      return (ret);                                                            \
    }                                                                          \
  }
#endif

/**
 * @brief        print_config_data
 * @param[in]    CONFIG_DATA_T        *config_data
 *
 * @return       status
 * @see
 */
static void print_config_data(CONFIG_DATA_T *config_data) {
  DEMOPRT((char *)"core_proc_type=%d\n", config_data->core_proc_type);
  DEMOPRT((char *)"test_data_type=%d\n", config_data->test_data_type);
  DEMOPRT((char *)"data_mem_type=%d\n", config_data->data_mem_type);
  DEMOPRT((char *)"nnie model path:%s\n", config_data->hikflow_model_path);
  DEMOPRT((char *)"test_image_list=%s\n", config_data->test_image_list);
  DEMOPRT((char *)"result_path=%s\n", config_data->result_path);
  DEMOPRT((char *)"n=%d\n", config_data->n);
  DEMOPRT((char *)"c=%d\n", config_data->c);
  DEMOPRT((char *)"h=%d\n", config_data->h);
  DEMOPRT((char *)"w=%d\n", config_data->w);
}

/**
 * @brief        check_plat_config                                // Verify
 * platform information
 * @param[in]   CONFIG_DATA_T        *config_data                 // Structure
 * of configuration information void                 *model_buffer // Virtual
 * memory address int                   model_size                  // Model
 * size int                  *flag_bin_info               // The flag that the
 * configuration information is recorded in the bin file
 * @return      status
 * @see
 */
static int check_plat_config(CONFIG_DATA_T *config_data, void *model_buffer,
                             int model_size, int *flag_bin_info) {
  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      NULL == config_data, "check config error: the config_data is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      NULL == model_buffer,
      "check model_buffer error: the model_buffer is null.", -1);
  FILE *model_file = NULL;
  char *start_point = NULL;
  char *ptr_real = NULL;
  char dev_info[] = "dev_info:";
  char plat_type[] = "plat_type:";
  char dtype[] = "dtype:";
  char dformat[] = "dformat:";
  char innum[] = "innum:";
  int dev_info_bin = 0;
  int plat_type_bin = 0;
  int dtype_bin = 0;
  int dformat_bin = 0;
  int in_blob_num_bin = 0;
  int batch_bin = 0;
  int channel_bin = 0;
  int height_bin = 0;
  int width_bin = 0;

  start_point = (char *)model_buffer + model_size - OPDEVSDK_MODEL_INFOLEN;

  if (0 == strncmp(start_point, dev_info, strlen(dev_info))) {
    *flag_bin_info = 1;
    ptr_real = start_point + strlen(dev_info);
    dev_info_bin = *(int *)ptr_real;
    DEMOPRT((char *)"has get bin dev_info: %d\n", dev_info_bin);

    ptr_real = ptr_real + strlen(plat_type) + sizeof(int);
    plat_type_bin = *(int *)ptr_real;
    DEMOPRT((char *)"has get bin plat_type: %d\n", plat_type_bin);

    ptr_real = ptr_real + strlen(dtype) + sizeof(int);
    dtype_bin = *(int *)ptr_real;
    DEMOPRT((char *)"has get bin dtype: %d\n", dtype_bin);

    ptr_real = ptr_real + strlen(dformat) + sizeof(int);
    dformat_bin = *(int *)ptr_real;
    DEMOPRT((char *)"has get bin dformat: %d\n", dformat_bin);

    ptr_real = ptr_real + strlen(innum) + sizeof(int);
    in_blob_num_bin = *(int *)ptr_real;
    DEMOPRT((char *)"has get bin in_blob_num: %d\n", in_blob_num_bin);

    ///< almost 8 blobs, 32 ints
    for (int i = 0; i < in_blob_num_bin; ++i) {
      ptr_real = ptr_real + sizeof(int);
      batch_bin = *(int *)ptr_real;
      DEMOPRT((char *)"has get bin batch: %d\n", batch_bin);

      ptr_real = ptr_real + sizeof(int);
      channel_bin = *(int *)ptr_real;
      DEMOPRT((char *)"has get bin channel: %d\n", channel_bin);

      ptr_real = ptr_real + sizeof(int);
      height_bin = *(int *)ptr_real;
      DEMOPRT((char *)"has get bin height: %d\n", height_bin);

      ptr_real = ptr_real + sizeof(int);
      width_bin = *(int *)ptr_real;
      DEMOPRT((char *)"has get bin width: %d\n", width_bin);
    }
  }

  if (*flag_bin_info == 1) ///< if bin carry info
  {
    ///< check platform
    if (dev_info_bin != PLATFORM_INFO) // H5：0，H7：1，KT2:2，G5：3
    {
      DEMOPRT((char *)"WARNING: The platform information is different between "
                      "CONFIG and BIN, please check.\n");
    }

    ///< CONFIG has 'core_proc_type' AND diff BIN, then choose BIN param in
    ///< default
    if (plat_type_bin != config_data->core_proc_type &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->core_proc_type) {
      DEMOPRT(
          (char *)"WARNING: The 'core_proc_type' is different between "
                  "CONFIG:%d and BIN:%d, will choose BIN param in default.\n",
          config_data->core_proc_type, plat_type_bin);
      config_data->core_proc_type = plat_type_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->core_proc_type) ///< CONFIG hasn't info, assign
                                            ///< value as BIN
    {
      config_data->core_proc_type = plat_type_bin;
    }

    if (dformat_bin != config_data->test_data_type &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->test_data_type) {
      DEMOPRT(
          (char *)"WARNING: The 'test_data_type' is different between "
                  "CONFIG:%d and BIN:%d, will choose BIN param in default.\n",
          config_data->test_data_type, dformat_bin);
      config_data->test_data_type = dformat_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->test_data_type) ///< CONFIG hasn't info, assign
                                            ///< value as BIN
    {
      config_data->test_data_type = dformat_bin;
    }

    if (batch_bin != config_data->n &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->n) {
      DEMOPRT((char *)"WARNING: The 'n' is different between CONFIG:%d and "
                      "BIN:%d, will choose BIN param in default.\n",
              config_data->n, batch_bin);
      config_data->n = batch_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->n) ///< CONFIG hasn't info, assign value as BIN
    {
      config_data->n = batch_bin;
    }

    if (channel_bin != config_data->c &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->c) {
      DEMOPRT((char *)"WARNING: The 'c' is different between CONFIG:%d and "
                      "BIN:%d, will choose BIN param in default.\n",
              config_data->c, channel_bin);
      config_data->c = channel_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->c) ///< CONFIG hasn't info, assign value as BIN
    {
      config_data->c = channel_bin;
    }

    if (height_bin != config_data->h &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->h) {
      DEMOPRT((char *)"WARNING: The 'h' is different between CONFIG:%d and "
                      "BIN:%d, will choose BIN param in default.\n",
              config_data->h, height_bin);
      config_data->h = height_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->h) ///< CONFIG hasn't info, assign value as BIN
    {
      config_data->h = height_bin;
    }

    if (width_bin != config_data->w &&
        OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG != config_data->w) {
      DEMOPRT((char *)"WARNING: The 'w' is different between CONFIG:%d and "
                      "BIN:%d, will choose BIN param in default.\n",
              config_data->w, width_bin);
      config_data->w = width_bin;
    } else if (OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG ==
               config_data->w) ///< CONFIG hasn't info, assign value as BIN
    {
      config_data->w = width_bin;
    }

  } else ///< if BIN don't carry info
  {
    if (1 == config_data->flag_config_all_param) ///< if BIN don't carry info,
                                                 ///< AND CONFIG has all info
    {
      DEMOPRT((char *)"WARNING: %s doesn't carry information, will choose "
                      "CONFIG param in default.\n",
              config_data->hikflow_model_path);
    } else { ///<  if BIN don't carry info, MEANWHILE CONFIG hasn't all info
      DEMOPRT(
          (char
               *)"ERROR: %s doesn't carry information, meanwhile your CONFIG is incomplete. please check your \
            BIN or CONFIG.\n",
          config_data->hikflow_model_path);
      return -1;
    }
  }
  return 0;
}

/**
 * @brief        demo_alg_createModel: create model
 * @param[in]    void                     **model_handle
 *               CONFIG_DATA_T            *config_data
 *               void                     **model_buffer
 *
 * @return       status
 * @see
 */
static int demo_alg_createModel(void **model_handle, CONFIG_DATA_T *config_data,
                                void **model_buffer) {
  FILE *model_file = NULL;
  int model_size = 0;
  int demo_ret = 0;
  int ret = 0;
  int flag_bin_info = 0;
  OPDEVSDK_HKA_MODEL_INFO_ST params_info;
  OPDEVSDK_HKA_MEM_TAB_ST hikflow_mem_tab_model[OPDEVSDK_HKA_MEM_TAB_NUM];
  OPDEVSDK_HKA_MEM_TAB_ST hikflow_mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM];

  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL != *model_handle),
                               "demo_alg_createModel: model_handle is null.",
                               -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      (NULL == config_data), "demo_alg_createModel: config_data is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      (NULL == config_data->hikflow_model_path),
      "demo_alg_createModel: config_data->hikflow_model_path is null.", -1);

  model_file = fopen(config_data->hikflow_model_path, "rb");
  if (NULL == model_file) {
    DEMOPRT((char *)"%s not find\n", config_data->hikflow_model_path);
    goto err;
  } else {
    DEMOPRT((char *)"read model bin success\n");
  }

  fseek(model_file, 0, SEEK_END);
  model_size = (unsigned int)ftell(model_file);
  rewind(model_file);

  if (model_size <= 0) {
    DEMOPRT((char *)"%s model_size<=0\n", config_data->hikflow_model_path);
    goto err;
  }

  int buf_size;
  buf_size = OPDEVSDK_HIKFLOW_SIZE_ALIGN(model_size, G5_PAGE_ALIGN);
  // FIXME:maybe need to modify
  ret = opdevsdk_mem_allocCache((void **)&g_model_phy_base, model_buffer,
                                (const char *)"Model_cache", buf_size);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_memAllocCache failed ret=%d\n", ret);
    goto err;
  }

  fread(*model_buffer, 1, model_size, model_file);

  if (NULL != model_file) {
    fclose(model_file);
    model_file = NULL;
  }

  ///< check platform info and bin/config info
  if (check_plat_config(config_data, *model_buffer, model_size,
                        &flag_bin_info) != 0) {
    DEMOPRT((char *)"check plat config failed\n");
    goto err;
  }

  print_config_data(config_data);

  params_info.model_buf = *model_buffer;
  params_info.model_size = model_size;

  params_info.proc_type =
      (OPDEVSDK_HKA_DL_PROCESS_TYPE_EN)config_data->core_proc_type;

  // register function of private layer
  params_info.custom_cb.get_model_memsize =
      (void *)Custom_Layer_GetModelMemsize;
  params_info.custom_cb.create_model = (void *)Custom_Layer_CreateModel;
  params_info.custom_cb.get_memsize = (void *)Custom_Layer_GetMemsize;
  params_info.custom_cb.create = (void *)Custom_Layer_Create;
  params_info.custom_cb.reshape = (void *)Custom_Layer_Reshape;
  params_info.custom_cb.forward = (void *)Custom_Layer_Forward;

  ///< get memory size
  ret = opdevsdk_hikflow_GetModelMemSize(&params_info, g_hikflow_mem_tab_model);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_hikflow_GetModelMemSize ret=0x%x\n", ret);
    goto err;
  } else {
    DEMOPRT((char *)"opdevsdk_hikflow_GetModelMemSize succeed\n");
  }

  // alloc model memory
  demo_ret = opdevsdk_hikflow_Device_Alloc_Memtab(g_hikflow_mem_tab_model,
                                                  OPDEVSDK_HKA_MEM_TAB_NUM);

  if (0 != demo_ret) {
    DEMOPRT((char *)"alloc_mem_tab model ret=0x%x\n", demo_ret);
    goto err;
  }

  ///< creat model handle
  ret = opdevsdk_hikflow_CreateModel(&params_info, g_hikflow_mem_tab_model,
                                     model_handle);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_hikflow_CreateModel failed. ret=0x%x\n", ret);
    goto err;
  } else {
    if (NULL != *model_handle) {
      DEMOPRT((char *)"opdevsdk_hikflow_CreateModel succeed.\n");
    } else {
      DEMOPRT((char *)"opdevsdk_hikflow_CreateModel failed.\n");
      goto err;
    }
  }

///< In addition to G5, free model_buffer
#if (PLATFORM_INFO != 3)     ///< H5：0，H7：1，KT2:2，G5：3
  if (NULL != *model_buffer) ///< for g5
  {
    opdevsdk_mem_free((void *)(PTR_VOID)g_model_phy_base, g_model_buffer);
    *model_buffer = NULL;
  }
#endif
  return OPDEVSDK_S_OK;

err:
  ///< release model
  if (NULL != *model_handle) {
    ret = opdevsdk_hikflow_ReleaseModel(*model_handle);
    if (0 != ret) {
      DEMOPRT((char *)"HIK_ALG_ReleaseModel ret=0x%x\n", ret);
    }
  }

  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab_model,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);

  if (NULL != model_file) {
    fclose(model_file);
  }

  return -1;
}

/**
 * @brief        demo_alg_createHandle
 * @param[in]    CONFIG_DATA_T               *config_data
 *               void                        *model_handle,
 *               OPDEVSDK_HIKFLOW_PARAM_ST   *param_info_net,
 *               void                        **net_handle
 * @return       status
 * @see
 */
static int demo_alg_createHandle(CONFIG_DATA_T *config_data, void *model_handle,
                                 OPDEVSDK_HIKFLOW_PARAM_ST *param_info_net,
                                 void **net_handle) {

  int ret;
  int demo_ret;

  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      (NULL == model_handle), "demo_alg_createHandle model_handle is null", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == config_data),
                               "demo_alg_createHandle config_data is null", -1);

  param_info_net->in_blob_num = 1;

  if (1 == config_data->test_data_type) {
    param_info_net->in_blob_param[0].src_format = OPDEVSDK_HKA_BGR; ///< BGR
  } else if (2 == config_data->test_data_type) {
    param_info_net->in_blob_param[0].src_format = OPDEVSDK_HKA_YVU420; ///< NV21
  } else if (4 == config_data->test_data_type) {
    param_info_net->in_blob_param[0].src_format = OPDEVSDK_HKA_YUV420; ///< NV12
  } else {
    DEMOPRT((char *)"ERROR: CONFIG 'test_data_type' just support ‘1’(BGR) or "
                    "‘2’(YVU420). Please check.\n");
    goto err;
  }

  param_info_net->in_blob_param[0].src_blob.space =
      OPDEVSDK_HKA_MEM_SPACE_CACHED; ///< kt2, g5
  param_info_net->in_blob_param[0].src_blob.dim = 4;
  param_info_net->in_blob_param[0].src_blob.type = OPDEVSDK_HKA_DATA_U08;
  param_info_net->in_blob_param[0].src_blob.shape[0] = config_data->n;
  param_info_net->in_blob_param[0].src_blob.shape[1] = config_data->c;
  param_info_net->in_blob_param[0].src_blob.shape[2] = config_data->h;
  param_info_net->in_blob_param[0].src_blob.shape[3] = config_data->w;
  param_info_net->in_blob_param[0].src_blob.stride[3] =
      config_data->w * sizeof(unsigned char);
  param_info_net->in_blob_param[0].src_blob.stride[2] = config_data->h;

  ///< set network handle
  param_info_net->handle_num = 1;
  param_info_net->handle[0].type = OPDEVSDK_HIKFLOW_MODEL_HANDLE;
  param_info_net->handle[0].handle = model_handle;

  ///< all the blobs in last layer are output
  param_info_net->out_blob_num = 1;
  param_info_net->out_blob_info[0].layer_idx = -1;
  param_info_net->out_blob_info[0].blob_idx = 0;

  ///< get network operation memory
  ret = opdevsdk_hikflow_GetMemSize(param_info_net, g_hikflow_mem_tab);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_hikflow_GetMemSize ret=0x%x\n", ret);
    goto err;
  } else {
    DEMOPRT((char *)"opdevsdk_hikflow_GetMemSize succeed\n");
  }

  ///< allocate network memory
  demo_ret = opdevsdk_hikflow_Device_Alloc_Memtab(g_hikflow_mem_tab,
                                                  OPDEVSDK_HKA_MEM_TAB_NUM);

  if (0 != demo_ret) {
    DEMOPRT((char *)"alloc_mem_tab net ret=0x%x\n", demo_ret);
    goto err;
  }

  ///< create network handle
  ret = opdevsdk_hikflow_Create(param_info_net, g_hikflow_mem_tab, net_handle);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_hikflow_Create ret=0x%x\n", ret);
    goto err;
  } else {
    if (*net_handle != NULL) {
      DEMOPRT((char *)"HIK_ALG_Create succeed\n");
    }
  }

  return OPDEVSDK_S_OK;

err:
  ///< release net
  if (NULL != *net_handle) {
    ret = opdevsdk_hikflow_Release(*net_handle);
    if (0 != ret) {
      DEMOPRT((char *)"HIK_ALG_Release ret=0x%x\n", ret);
    }
  }

  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);

  return -1;
}

/**
 * @brief        demo_alg_Process
 * @param[in]    void                                *net_handle,
 *               OPDEVSDK_HIKFLOW_PARAM_ST           *param_info_net
 *               OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST* hkann_out
 *               CONFIG_DATA_T                       *config_data
 *               void                                *pfrm,
 *               unsigned int                        num,
 *               OUTPUT_BOX_INFO                     *box_info
 *               unsigned long long                  *total
 * @return       status
 * @see
 */
static int demo_alg_Process(void *net_handle,
                            OPDEVSDK_HIKFLOW_PARAM_ST *param_info_net,
                            OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST *hkann_out,
                            CONFIG_DATA_T *config_data, void *pfrm,
                            unsigned int num, unsigned long long *total) {
  int data_size = 0;
  unsigned char *data_temp = NULL;
  unsigned long long phy_base = 0;
  float im_info[3];
  int ret = 0;
  void *test_img_data = NULL;
  int blob_count = 0;
  float result_p[16];
  struct timeval time_begin, time_end;
  unsigned long long one_time = 0;

  ///< check the input point
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == net_handle),
                               "demo_alg_Process: net_handle is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == config_data),
                               "demo_alg_Process: config_data is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == param_info_net),
                               "demo_alg_Process: param_info_net is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == pfrm),
                               "demo_alg_Process: pfrm is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((NULL == hkann_out),
                               "demo_alg_Process: hkann_out is null.", -1);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((0 == num), "demo_alg_Process: num = 0.", -1);

  ///< nnie forward module do not support image scale, so set im_info[2] = 1.0
  im_info[0] = config_data->h * 1.0f;
  im_info[1] = config_data->w * 1.0f;
  im_info[2] = 1.0;

  OPDEVSDK_HIKFLOW_FORWARD_IN_INFO_ST hkann_in = {0};

  ///< configuration of input params
  hkann_in.in_blob_num = 1;

  hkann_in.in_blob[0].src_format = param_info_net->in_blob_param[0].src_format;
  hkann_in.in_blob[0].src_blob.space =
      param_info_net->in_blob_param[0].src_blob.space;
  hkann_in.in_blob[0].src_blob.format = OPDEVSDK_HKA_FORMAT_NCHW;
  ///< hkann_in.in_blob[0].src_blob.type   =
  ///< param_info_net->in_blob_param[0].src_blob.type;
  hkann_in.in_blob[0].src_blob.type = OPDEVSDK_HKA_DATA_U08;

  hkann_in.in_blob[0].src_blob.dim =
      param_info_net->in_blob_param[0].src_blob.dim;
  hkann_in.in_blob[0].src_blob.data = pfrm;
  hkann_in.in_blob[0].src_blob.shape[0] = num;
  hkann_in.in_blob[0].src_blob.shape[1] = config_data->c;
  hkann_in.in_blob[0].src_blob.shape[2] = config_data->h;
  hkann_in.in_blob[0].src_blob.shape[3] = config_data->w;

  hkann_in.in_blob[0].src_blob.stride[3] =
      config_data->w * sizeof(unsigned char);
  hkann_in.in_blob[0].src_blob.stride[2] = config_data->h;

  ///< fd process
  gettimeofday(&time_begin, NULL);
  ret = opdevsdk_hikflow_Process(net_handle, 0, &hkann_in, sizeof(hkann_in),
                                 hkann_out, sizeof(*hkann_out));
  gettimeofday(&time_end, NULL);
  one_time = 1000000 * (time_end.tv_sec - time_begin.tv_sec) +
             (time_end.tv_usec - time_begin.tv_usec);
  *total += one_time;
  if (0 != ret) {
    DEMOPRT((char *)"HIK_ALG_Process failed ret=0x%x\n", ret);
    return -2;
  } else {
    DEMOPRT((char *)"opdevsdk_hikflow_Process ret=0x%x, cost %llu us\n", ret,
            one_time);
  }
  return OPDEVSDK_S_OK;
}

/**
 * @brief        parse_configure_info
 * @param[in]    char                *config_file_path
 *               CONFIG_DATA_T       *config_data
 * @return       status
 * @see
 */
static int parse_configure_info(char *config_file_path,
                                CONFIG_DATA_T *config_data) {
  int res = 0;
  FILE *configure_file = NULL;
  config_data->flag_config_all_param = 1; ///< flag: config has all info ?

  configure_file = fopen(config_file_path, "rb");
  if (NULL == configure_file) {
    DEMOPRT((char *)"configure file %s not exit\n", config_file_path);
    return -1;
  }

  ///< parse processor type
  res = fscanf(configure_file, "core_proc_type=%d\n",
               &config_data->core_proc_type);
  if (1 != res) {
    config_data->core_proc_type = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }

  ///< parse input data type of network test
  res = fscanf(configure_file, "test_data_type=%d\n",
               &config_data->test_data_type);
  if (1 != res) {
    config_data->test_data_type = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }

  ///< parse input data memory type of network test
  res =
      fscanf(configure_file, "data_mem_type=%d\n", &config_data->data_mem_type);
  if (1 != res) {
    config_data->data_mem_type = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }

  ///< parse nnie model path
  res = fscanf(configure_file, "hikflow_model_path=%s\n",
               &config_data->hikflow_model_path[0]);
  if (1 != res) {
    DEMOPRT((char *)"Error: There is no 'hikflow_model_path' in your config, "
                    "This param must have.\n");
    return -1;
  }

  ///< parse test images path
  res = fscanf(configure_file, "test_image_list=%s\n",
               &config_data->test_image_list[0]);
  if (1 != res) {
    DEMOPRT((char *)"Error: There is no 'test_image_list' in your config, This "
                    "param must have.\n");
    return -1;
  }

  ///< parse output file path
  res =
      fscanf(configure_file, "result_path=%s\n", &config_data->result_path[0]);
  if (1 != res) {
    DEMOPRT((char *)"Error: There is no 'result_path' in your config, This "
                    "param must have.\n");
    return -1;
  }

  res = fscanf(configure_file, "n=%d\n", &config_data->n);
  if (1 != res) {
    config_data->n = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }
  res = fscanf(configure_file, "c=%d\n", &config_data->c);
  if (1 != res) {
    config_data->c = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }
  res = fscanf(configure_file, "h=%d\n", &config_data->h);
  if (1 != res) {
    config_data->h = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }
  res = fscanf(configure_file, "w=%d\n", &config_data->w);
  if (1 != res) {
    config_data->w = OPDEVSDK_NOT_FIND_PARAM_IN_CONFIG;
    config_data->flag_config_all_param &= 0;
  }

  fclose(configure_file);
  configure_file = NULL;

  return 0;
}

int demo_alg_init(char *config_file_path) {
  int ret = 0;
  if (parse_configure_info(config_file_path, &g_config_data) != 0) {
    DEMOPRT((char *)"parse configure info failed\n");
    return -1;
  }

  ///< load model
  ret = demo_alg_createModel(&g_model_handle, &g_config_data, &g_model_buffer);
  OPDEVSDK_HIKFLOW_CHECK_ERROR((OPDEVSDK_S_OK != ret),
                               "demo_alg_init: demo_alg_createModel error", -1);

  ///< creat handle
  ret = demo_alg_createHandle(&g_config_data, g_model_handle, &g_param_info_net,
                              &g_net_handle);
  OPDEVSDK_HIKFLOW_CHECK_ERROR(
      (OPDEVSDK_S_OK != ret), "demo_alg_init: demo_alg_createHandle error", -1);
  return OPDEVSDK_S_OK;
}

/**
 * @brief        demo_alg_proc_fromCamera
 * @param[in]    OPDEVSDK_VIDEO_FRAME_INFO_ST        *pfrm
 *               OPDEVSDK_POS_TARGET_LIST_INFO_ST    *ptarget
 * @return       status
 * @see
 */
int demo_alg_proc_fromCamera(OPDEVSDK_VIDEO_FRAME_INFO_ST *pfrm,
                             OPDEVSDK_POS_TARGET_LIST_INFO_ST *ptarget,
                             int check_weight_limit) {
  // int check_weight_limit  地磅所在的行的最大值
  int check_flag = -1;
  int ret = 0;
  unsigned long long total = 0;

  OUTPUT_BOX_INFO *box_info = NULL;
  OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST hkann_out = {0};
  memset(&hkann_out, 0, sizeof(OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST));

  ret = demo_alg_Process(g_net_handle, &g_param_info_net, &hkann_out,
                         &g_config_data, pfrm->yuvFrame.pVirAddr[0], 1, &total);
  if (OPDEVSDK_S_OK != ret) {
    DEMOPRT((char *)"demo_alg_Process error= 0x%x \n", ret);
    goto err;
  }

  ///< detection output
  // DEMOPRT((char*)"----------------------------------------------------------------\n");
  // DEMOPRT((char*)"this is detection output \n    the out format is
  // class,score,x,y,width,height,batch_index\n"); DEMOPRT((char*)"if the model
  // is not detection,or the out format is different,\n    please change it
  // \n");
  // DEMOPRT((char*)"----------------------------------------------------------------\n");
  int detectnum;
  detectnum = hkann_out.output_blob[0].shape[0];
  for (int n = 0; n < detectnum; n++) {
    box_info = (OUTPUT_BOX_INFO *)hkann_out.output_blob[0].data;
    box_info = box_info + n;

    float class_type = box_info->class_type;

    if ((int)class_type == 2 || (int)class_type == 5 || (int)class_type == 6 ||
        (int)class_type == 7) {
      DEMOPRT((char *)"*********there is a car here*******");
      DEMOPRT((char *)"current bounding box: %d\n", n);
      DEMOPRT((char *)"class = %f\n", box_info->class_type);
      DEMOPRT((char *)"score  = %f\n", box_info->score);
      DEMOPRT((char *)"x      = %f\n", box_info->bbox.x);
      DEMOPRT((char *)"y      = %f\n", box_info->bbox.y);
      DEMOPRT((char *)"w      = %f\n", box_info->bbox.w);
      DEMOPRT((char *)"h      = %f\n", box_info->bbox.h);
      DEMOPRT((char *)"batch  = %f\n", box_info->batch_idx);
      float y_max = box_info->bbox.y + box_info->bbox.h;
      DEMOPRT((char *)"y_max     = %f\n", y_max);
      DEMOPRT((char *)"check_weight_limit     = %f\n", check_weight_limit);
      if (y_max > check_weight_limit) {
        check_flag = 0;
        continue;
        DEMOPRT((char *)"*********the car is outside the weight*******");
      } else {
        check_flag = 1;
        continue;
        DEMOPRT((char *)"*********the car is on the weight*******");
      }
      break;
    } else {
      check_flag = -1;
    }

    if (ptarget->tgtList.tgtNum < MAX_OUTPUT_BOX_NUM) {
      ptarget->tgtList.tgtNum++;
      ptarget->tgtList.pTgt[n].region.pointNum = 4;
      ptarget->tgtList.pTgt[n].id = ptarget->tgtList.tgtNum;
      ptarget->tgtList.pTgt[n].trace_time = 0;
      ptarget->tgtList.pTgt[n].color = 0;
      ptarget->tgtList.pTgt[n].region.point[0].x =
          box_info->bbox.x / g_config_data.w;
      ptarget->tgtList.pTgt[n].region.point[0].y =
          box_info->bbox.y / g_config_data.h;
      ptarget->tgtList.pTgt[n].region.point[1].x =
          box_info->bbox.x / g_config_data.w +
          box_info->bbox.w / g_config_data.w;
      ptarget->tgtList.pTgt[n].region.point[1].y =
          box_info->bbox.y / g_config_data.h;
      ptarget->tgtList.pTgt[n].region.point[2].x =
          box_info->bbox.x / g_config_data.w +
          box_info->bbox.w / g_config_data.w;
      ptarget->tgtList.pTgt[n].region.point[2].y =
          box_info->bbox.y / g_config_data.h +
          box_info->bbox.h / g_config_data.h;
      ptarget->tgtList.pTgt[n].region.point[3].x =
          box_info->bbox.x / g_config_data.w;
      ptarget->tgtList.pTgt[n].region.point[3].y =
          box_info->bbox.y / g_config_data.h +
          box_info->bbox.h / g_config_data.h;
      // DEMOPRT((char*)"n %d alg_w %d alg_h %d x %f y %f w %f h %f  target num
      // %d\n",
      //     n, g_config_data.w, g_config_data.h, box_info->bbox.x,
      //     box_info->bbox.y, box_info->bbox.w, box_info->bbox.h,
      //     ptarget->tgtList.tgtNum);
    }
  }
  ptarget->timeStamp = pfrm->timeStamp;
  return check_flag;

  int ert;
err:

  ///< release network
  if (NULL != g_net_handle) {
    ert = opdevsdk_hikflow_Release(g_net_handle);
    if (0 != ert) {
      DEMOPRT((char *)"opdevsdk_hikflow_Release ret=0x%x\n", ert);
    }
  }

  ///< release model
  if (NULL != g_model_handle) {
    ert = opdevsdk_hikflow_ReleaseModel(g_model_handle);
    if (0 != ert) {
      DEMOPRT((char *)"opdevsdk_hikflow_ReleaseModel ret=0x%x\n", ert);
    }
  }

  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab_model,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);
  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);

  if (NULL != g_model_buffer) {
    opdevsdk_mem_free((void *)(PTR_VOID)g_model_phy_base, g_model_buffer);
  }

  return -1;
}

/**
 * @brief        demo_alg_proc_fromFile
 * @param[in]
 * @return       status
 * @see
 */
int demo_alg_proc_fromFile() {
  int ret = 0;
  unsigned long long total = 0;

  OUTPUT_BOX_INFO *box_info;
  OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST hkann_out;
  memset(&hkann_out, 0, sizeof(hkann_out));

  FILE *test_file = NULL;
  void *test_img_data = NULL;
  char file_name[256] = {'\0'};
  int total_pic_num = 0;
  unsigned char *data_temp = NULL;
  unsigned char *current_processing_pic = NULL;
  int data_size = 0;
  int i = 0, j = 0, k = 0;
  FILE *pic_file = NULL;
  long one_time = 0, total_time = 0;
  int blob_count = 0;
  float im_info[3];
  int n = 0, c = 0, h = 0, w = 0;
  unsigned long long phy_base = 0;
  test_file = fopen(g_config_data.test_image_list, "rb");
  if (NULL == test_file) {
    DEMOPRT((char *)"fopen %s failed\n", g_config_data.test_image_list);
    goto err;
  }

  while (fgets(file_name, 256, test_file) != NULL) {
    total_pic_num++;
  }
  fseek(test_file, 0, SEEK_SET);

  DEMOPRT((char *)"total_pic_num:%d\n", total_pic_num);

  ///< allocate input images memory
  if (g_param_info_net.in_blob_param[0].src_format ==
      OPDEVSDK_HKA_BGR) ///< bgr format memory size
  {
    data_size =
        g_config_data.n * g_config_data.w * g_config_data.c * g_config_data.h;
  } else ///< nv21 or nv12 format memory size
  {
    data_size = g_config_data.n * g_config_data.w *
                (g_config_data.h + (g_config_data.h >> 1));
  }

  // FIXME: maybe need to modify
  ret = opdevsdk_mem_allocCache((void **)&phy_base, (void **)&(test_img_data),
                                (const char *)"mmcache", data_size);
  if (0 != ret) {
    DEMOPRT((char *)"opdevsdk_memAllocCache failed ret=%d\n", ret);
    goto err;
  }

  // FIXME: char res_file_name[256] = { 0 };
  char res_file_name[256];
  memset(res_file_name, 0, 256);

  for (j = 0; j < total_pic_num; j = j + g_config_data.n) {
    data_temp = (unsigned char *)test_img_data;
    current_processing_pic = data_temp;

    for (k = 0; k < g_config_data.n; k++) {
      if ((j + k) >= total_pic_num) {
        DEMOPRT((char *)"all pic has process.\n");
        break;
      }

      fscanf(test_file, "%s", file_name);
      file_name[strlen(file_name) + 1] = '\0';

      DEMOPRT((char *)"process pic : %s\n", file_name);

      pic_file = fopen(file_name, "rb");
      if (NULL == pic_file) {
        DEMOPRT((char *)"[%s]pic file can't find\n", file_name);
        goto err;
      }
      fseek(pic_file, 0, SEEK_SET);

      ///< read bgr images
      if (g_param_info_net.in_blob_param[0].src_format == OPDEVSDK_HKA_BGR) {
        data_size = g_config_data.c * g_config_data.h * g_config_data.w;
      } else ///< read nv21 images
      {
        data_size =
            (g_config_data.h + (g_config_data.h >> 1)) * g_config_data.w;
      }
      fread(data_temp, data_size, 1, pic_file);
      data_temp += data_size;

      if (NULL != pic_file) {
        fclose(pic_file);
        pic_file = NULL;
      }
    }

    ret = demo_alg_Process(g_net_handle, &g_param_info_net, &hkann_out,
                           &g_config_data, test_img_data, k, &total);
    if (ret != 0) {
      DEMOPRT((char *)"demo_alg_Process error= 0x%x \n", ret);
      goto err;
    }

    ///< general output format
    ret =
        snprintf(res_file_name, sizeof(res_file_name), "picture_%d_out.txt", j);
    FILE *result_fp = NULL;
    result_fp = fopen(res_file_name, "w");
    float score = 0;
    // DEMOPRT((char*)"----------------------------------------------------------------\n");
    // DEMOPRT((char*)"this is general output \n");
    // DEMOPRT((char*)"you can get result in %s\n",res_file_name);
    // DEMOPRT((char*)"----------------------------------------------------------------\n");
    int n =
        hkann_out.output_blob[0].shape[0] * hkann_out.output_blob[0].shape[1] *
        hkann_out.output_blob[0].shape[2] * hkann_out.output_blob[0].shape[3];
    DEMOPRT((char *)"blob_num = %d,shape[0] = %d,shape[1] =%d,shape[2] "
                    "=%d,shape[3] =%d\n",
            hkann_out.blob_num, hkann_out.output_blob[0].shape[0],
            hkann_out.output_blob[0].shape[1],
            hkann_out.output_blob[0].shape[2],
            hkann_out.output_blob[0].shape[3]);
    // for (int c = 0; c < n; c++)
    // {
    //     score = ((float *)(hkann_out.output_blob[0].data))[c];
    //     fprintf(result_fp, "%f\n", score);
    //
    // }

    ///< detection output
    DEMOPRT((char *)"----------------------------------------------------------"
                    "------\n");
    DEMOPRT((char *)"this is detection output \n    the out format is "
                    "class,score,x,y,width,height,batch_index\n");
    DEMOPRT((char *)"if the model is not detection,or the out format is "
                    "different,\n    please change it \n");
    DEMOPRT((char *)"----------------------------------------------------------"
                    "------\n");
    int detectnum = hkann_out.output_blob[0].shape[0];
    for (i = 0; i < detectnum; i++) {
      box_info = (OUTPUT_BOX_INFO *)hkann_out.output_blob[0].data;
      box_info = box_info + i;
      DEMOPRT((char *)"current bounding box: %d\n", i);
      DEMOPRT((char *)"class  = %f\n", box_info->class_type);
      DEMOPRT((char *)"score   = %f\n", box_info->score);
      DEMOPRT((char *)"x       = %f\n", box_info->bbox.x);
      DEMOPRT((char *)"y       = %f\n", box_info->bbox.y);
      DEMOPRT((char *)"w       = %f\n", box_info->bbox.w);
      DEMOPRT((char *)"h       = %f\n", box_info->bbox.h);
      DEMOPRT((char *)"batch   = %f\n", box_info->batch_idx);

    // TODO: 可能会报错
      Mat rawToMat = Mat(640, 640, CV_8UC3, (uchar *)current_processing_pic);
      float x_min = box_info->bbox.x;
      float y_min = box_info->bbox.y;
      float x_max = box_info->bbox.x + box_info->bbox.w;
      float y_max = box_info->bbox.y + box_info->bbox.h;
      DEMOPRT((char *)"x_min     = %f\n", x_min);
      DEMOPRT((char *)"y_min     = %f\n", y_min);
      DEMOPRT((char *)"x_max     = %f\n", x_max);
      DEMOPRT((char *)"y_max     = %f\n", y_max);

      Mat plate =
          rawToMat(Rect(x_min, y_min, x_max - x_min + 1, y_max - y_min + 1));
      // 符合CRNN模型输入的尺寸
      Mat plate_resize = Mat::zeros(Size(168, 48), plate.type());
      // 计算截取的车牌的宽高比
      double aspect_ratio = static_cast<double>(plate.cols) / plate.rows;
      int new_width = 168;
      int new_height = static_cast<int>(new_width / aspect_ratio);
      if (new_height > 48) {
        new_height = 48;
        new_width = static_cast<int>(new_height * aspect_ratio);
      }
      DEMOPRT((char *)"crop_plate(keep aspect ratio) new width: %d, new height: %d\n", new_width, new_height);

      // 将车牌缩放到指定尺寸
      Mat temp;
      resize(plate, temp, Size(new_width, new_height), 0, 0, INTER_CUBIC);
      // 计算车牌在新图像中的中心位置
      int x_offset = (168 - new_width) / 2;
      int y_offset = (48 - new_height) / 2;
      // 将车牌放到新图像中
      temp.copyTo(plate_resize(Rect(x_offset, y_offset, new_width, new_height)));

      DEMOPRT((char *)"plate_resize width = %d\n", plate_resize.cols);
      DEMOPRT((char *)"plate_resize height = %d\n", plate_resize.rows);

      // XXX: 以下为验证代码
      unsigned char* plate_data = (unsigned char*)plate_resize.data;
      // 打印前2个GBR通道的值，openCV的存储方式是BGR[B, G, R, B, G, R], 验证是否正确
      for (int i = 0; i < 6; i++) {
        DEMOPRT((char *)"plate_data[%d] = %d\n", i, plate_data[i]);
      }
    }

    DEMOPRT((char *)"----------------------------------------------------------"
                    "------\n");
    if (NULL != result_fp) {
      fclose(result_fp);
    }
  }
  DEMOPRT((char *)"Process avg time:%llu us\n", total / total_pic_num);

err:
  ///< release network
  if (NULL != g_net_handle) {
    ret = opdevsdk_hikflow_Release(g_net_handle);

    if (0 != ret) {
      DEMOPRT((char *)"opdevsdk_hikflow_Release ret=0x%x\n", ret);
    }
  }

  ///< release model
  if (NULL != g_model_handle) {
    ret = opdevsdk_hikflow_ReleaseModel(g_model_handle);
    if (0 != ret) {
      DEMOPRT((char *)"opdevsdk_hikflow_ReleaseModel ret=0x%x\n", ret);
    }
  }

  demo_alg_releaseBuffer();

  if (NULL != test_img_data) {
    opdevsdk_mem_free((void *)(PTR_VOID)phy_base, test_img_data);
  }

  if (NULL != test_file) {
    fclose(test_file);
  }

  if (NULL != pic_file) {
    fclose(pic_file);
  }

  opdevsdk_sys_deinit();

  return OPDEVSDK_S_OK;
}

/**
 * @brief        demo_alg_deinit
 * @param[in]
 * @return       status
 * @see
 */
int demo_alg_deinit() {
  int hikflow_ret = 0;

  ///< release net resource
  if (NULL != g_net_handle) {
    hikflow_ret = opdevsdk_hikflow_Release(g_net_handle);
    DEMOPRT((char *)"HIK_ALG_ReleaseHandle ret=0x%x\n", hikflow_ret);
  }

  ///< release model
  if (NULL != g_model_handle) {
    hikflow_ret = opdevsdk_hikflow_ReleaseModel(g_model_handle);
    DEMOPRT((char *)"HIK_ALG_ReleaseModel ret=0x%x\n", hikflow_ret);
  }

  return OPDEVSDK_S_OK;
}

/**
 * @brief        demo_alg_releaseBuffer
 * @param[in]
 * @return       status
 * @see
 */
int demo_alg_releaseBuffer() {
  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab_model,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);
  opdevsdk_hikflow_Device_Free_Memtab(g_hikflow_mem_tab,
                                      OPDEVSDK_HKA_MEM_TAB_NUM);

  if (g_model_buffer != NULL) {
    opdevsdk_mem_free((void *)(PTR_VOID)g_model_phy_base, g_model_buffer);
    g_model_buffer = NULL;
    DEMOPRT((char *)"free g_model_buffer\n");
  }
  return OPDEVSDK_S_OK;
}

int demo_alg_get_res(int *alg_w, int *alg_h) {
  if (NULL == alg_w || NULL == alg_h) {
    DEMOPRT((char *)"free g_model_buffer\n");
    return -1;
  }
  *alg_w = g_config_data.w;
  *alg_h = g_config_data.h;
  return OPDEVSDK_S_OK;
}
