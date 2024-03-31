/**@file     custom_YoloV5_s6_Region_layer.c
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Implementation of YoloV5_s6_Region layer
 * 
 * @author   
 * @date     
 * @version  V1.5.0
 * 
 * @note     
 * @warning  
 */
#include <float.h>
#include <string.h>
#include <stdio.h>
#include "custom_YoloV5_s6_Region_layer.h"
#include "YoloV5_s6_Region.h"

#define CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE          128
#define CUSTOM_YOLOV5_S6_REGION_ALIGN(size)             (((size) + ((128) - 1)) & (~((128) - 1)))
#define OPC_SIZE_ALIGN(size, align)             ((((int)size) + ((align)-1)) & (~((align)-1)))

static int dim1[4] = {3,3,3,3};
static int dim2[4] = {0,1,2,3};
static int dim3[4] = {1,2,3,3};
static int dim4[4] = {0,1,2,3};

void*   OPC_RUNTIME_set_mem_addr(void* addr);

/**@fn         CUSTOM_alloc_buffer
*  @brief      alloc memory buffer
*  @param[in]  OPDEVSDK_HKA_BUF_ST           *ivs_buf    memory buffer to be allocated
               unsigned int                   size
               OPDEVSDK_HKA_MEM_ALIGNMENT_EN  alignment  informations  of current layer
               unsigned int                   clean      memory size and attribution needed for current layer
*  @return     HRESULT
*/
static void  *CUSTOM_alloc_buffer(OPDEVSDK_HKA_BUF_ST          *ivs_buf,
                                  unsigned int                  size,
                                  OPDEVSDK_HKA_MEM_ALIGNMENT_EN alignment,
                                  unsigned int                  clean)
{
    int              align = (int)alignment;
    void            *buf   = NULL;
    unsigned int     free_size = 0;
    buf = (void *)(((size_t)ivs_buf->cur_pos + (align - 1)) & (~(align - 1)));
    ///< Calculate the starting address
    if ((size_t)ivs_buf->end < (size_t)buf)
    {
        buf = NULL;
        return buf;
    }
    ///< Calculate the remaining memory buffer size
    free_size = (char *)ivs_buf->end - (char *)buf;
    ///< Insufficient memory space, return null pointer
    if (free_size < size)
    {
        buf = NULL;
    }
    else
    {
        ///< Clean up allocated memory
        if (clean)
        {
            memset(buf, 0, size);
        }

        ///< Update the free memory address
        ivs_buf->cur_pos = (void *)((char *)buf + size);
    }
    return buf;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_reshape
*  @brief      compute the shape of the outblob of YOLOV5_S6_REGION layer
*  @param[i/o] void                       *handle   handle of YOLOV5_S6_REGION layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of YOLOV5_S6_REGION layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_reshape(void                      *handle,
                            OPDEVSDK_HIKFLOW_LDATA_ST *ld)
{
    int sts         = HIKFLOW_STS_OK;
    OPDEVSDK_YOLOV5_S6_REGION_LAYER_T  *yolov5_s6_region_layer = (OPDEVSDK_YOLOV5_S6_REGION_LAYER_T *)handle;

    ///< define the output of reshape function
    int outshape[4] = {0};
    ///< define the memory of reshape function
    int assist_mem_size = 0;
    int in1_n = ld->input_blobs[0]->shape[dim4[0]];
    int in1_c = ld->input_blobs[0]->shape[dim4[1]];
    int in1_h = ld->input_blobs[0]->shape[dim4[2]];
    int in1_w = ld->input_blobs[0]->shape[dim4[3]];
    int in2_n = ld->input_blobs[1]->shape[dim4[0]];
    int in2_c = ld->input_blobs[1]->shape[dim4[1]];
    int in2_h = ld->input_blobs[1]->shape[dim4[2]];
    int in2_w = ld->input_blobs[1]->shape[dim4[3]];
    int in3_n = ld->input_blobs[2]->shape[dim4[0]];
    int in3_c = ld->input_blobs[2]->shape[dim4[1]];
    int in3_h = ld->input_blobs[2]->shape[dim4[2]];
    int in3_w = ld->input_blobs[2]->shape[dim4[3]];
    int in4_n = ld->input_blobs[3]->shape[dim4[0]];
    int in4_c = ld->input_blobs[3]->shape[dim4[1]];
    int in4_h = ld->input_blobs[3]->shape[dim4[2]];
    int in4_w = ld->input_blobs[3]->shape[dim4[3]];
	
    ///< define the hyperparams of reshape function
    int num_anchors = yolov5_s6_region_layer->model->num_anchors;
    ///< call reshape function
    sts = YoloV5_s6_Region_Reshape( in1_n, in1_c, in1_h, in1_w, in2_n, in2_c, in2_h, in2_w, in3_n, in3_c, in3_h, in3_w, in4_n, in4_c, in4_h, in4_w, num_anchors, &assist_mem_size, outshape );
    CHECK_ERROR(HIKFLOW_STS_OK != sts, "CUSTOM_YOLOV5_S6_REGION_reshape YoloV5_s6_Region_Reshape failed!", HIKFLOW_STS_ERR_CUS_LAYER_RESHAPE_FAILED);
    ///< set the output blob
    int j = 0;
    for (j = 0;j < 4;j++)
    {
        if (outshape[j] == 0)
        {
           outshape[j] = 1;
        }
    }
    ld->output_blobs[0].dim      = 4;
    ld->output_blobs[0].type     = OPDEVSDK_HKA_DATA_F32; ///< custom layer type is float32
    ld->output_blobs[0].format   = OPDEVSDK_HKA_FORMAT_NCHW;
    ld->output_blobs[0].shape[3] = outshape[dim4[3]];
    ld->output_blobs[0].shape[2] = outshape[dim4[2]];
    ld->output_blobs[0].shape[1] = outshape[dim4[1]];
    ld->output_blobs[0].shape[0] = outshape[dim4[0]];
    ld->output_blobs[0].stride[3]= OPC_SIZE_ALIGN(ld->output_blobs[0].shape[3] * sizeof(float), 16);
    LOG_INFO("ld->output_blobs\ndim:%d type:%d format:%d\nshpae[0]:%d shpae[1]:%d shpae[2]:%d shpae[3]:%d stride:%d\n",
             ld->output_blobs[0].dim, ld->output_blobs[0].type, ld->output_blobs[0].format,
             ld->output_blobs[0].shape[0], ld->output_blobs[0].shape[1],
             ld->output_blobs[0].shape[2], ld->output_blobs[0].shape[3],
             ld->output_blobs[0].stride[3]);
    ///< set the memory size
    yolov5_s6_region_layer->scratch_memory_size = assist_mem_size;
    return HIKFLOW_STS_OK;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_init_model
*  @brief      get initialization of YOLOV5_S6_REGION model params
*  @param[in]  const char                   *hyperparams    hyperparams of YOLOV5_S6_REGION layer
               const char                   *param_blobs    param_blobs of YOLOV5_S6_REGION layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld             informations of YOLOV5_S6_REGION layer
               int                          init_blob       initial of blob
               OPDEVSDK_HKA_BUF_ST          *malloc_buf     malloc buf

*  @param[out] OPDEVSDK_YLOUT_MODEL_T       *yolov5_s6_region_model yolov5_s6_region_model model inforamtion
*  @return     HRESULT
*/
static int CUSTOM_YOLOV5_S6_REGION_init_model(const char                         *hyperparams,
                                      const char                         *param_blobs,
                                      OPDEVSDK_HIKFLOW_LMODEL_ST         *ld,
                                      OPDEVSDK_YOLOV5_S6_REGION_MODEL_T          *yolov5_s6_region_model,
                                      int                                 init_blob,
                                      OPDEVSDK_HKA_BUF_ST                *malloc_buf)
{
    int sts         = HIKFLOW_STS_OK;
    int r           = 0;
    int blob_size   = 0;
    int malloc_size = 0;
    int i           = 0;
    const char *ptr_hp        = NULL;
    const char *param_str     = "param_str";
    const char *ptr_param_str = NULL;
    ///< use for repeated hyper param
    const char *comma_ptr     = NULL;
    const char *comma         = ",";
    const char *num_anchors = "'num_anchors'";
    const char *num_classes = "'num_classes'";
    const char *conf_thresh = "'conf_thresh'";
    const char *nms_thresh = "'nms_thresh'";
    ///< set the hyperparams
    ///< get num_anchors
    ptr_hp = strstr(hyperparams, num_anchors);
    CHECK_ERROR(NULL == ptr_hp, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(strstr)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_NULL_PTR);
    r = sscanf(ptr_hp + strlen(num_anchors) + 2, "%d", &yolov5_s6_region_model->num_anchors);
    CHECK_ERROR(1 != r, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(sscanf)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_FAILED);
    ///< get num_classes
    ptr_hp = strstr(hyperparams, num_classes);
    CHECK_ERROR(NULL == ptr_hp, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(strstr)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_NULL_PTR);
    r = sscanf(ptr_hp + strlen(num_classes) + 2, "%d", &yolov5_s6_region_model->num_classes);
    CHECK_ERROR(1 != r, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(sscanf)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_FAILED);
    ///< get conf_thresh
    ptr_hp = strstr(hyperparams, conf_thresh);
    CHECK_ERROR(NULL == ptr_hp, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(strstr)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_NULL_PTR);
    r = sscanf(ptr_hp + strlen(conf_thresh) + 2, "%f", &yolov5_s6_region_model->conf_thresh);
    CHECK_ERROR(1 != r, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(sscanf)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_FAILED);
    ///< get nms_thresh
    ptr_hp = strstr(hyperparams, nms_thresh);
    CHECK_ERROR(NULL == ptr_hp, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(strstr)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_NULL_PTR);
    r = sscanf(ptr_hp + strlen(nms_thresh) + 2, "%f", &yolov5_s6_region_model->nms_thresh);
    CHECK_ERROR(1 != r, "CUSTOM_YOLOV5_S6_REGION_init_model : get param failed(sscanf)!\n", HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_FAILED);
    return HIKFLOW_STS_OK;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_GetModelMemsize
*  @brief      get the memory size of YoloV5_s6_Region layer for model creation
*  @param[in]  const char                   *hyperparams hyperparams of YoloV5_s6_Region layer
               const char                   *param_blobs param_blobs of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld          informations of YoloV5_s6_Region layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory size and attribution needed for current layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_GetModelMemsize(const char                 *hyperparams,
                                    const char                 *param_blobs,
                                    OPDEVSDK_HIKFLOW_LMODEL_ST *ld,
                                    OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM])
{
    int sts                 = HIKFLOW_STS_OK;
    int malloc_size         = 0;
    OPDEVSDK_HKA_MEM_TAB_ST *malloc_handle_tab = mem_tab;
    OPDEVSDK_YOLOV5_S6_REGION_MODEL_T yolov5_s6_region_model = {0};
    CHECK_ERROR(NULL == hyperparams, "CUSTOM_YOLOV5_S6_REGION_GetModelMemsize error:hyperparams is null\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == ld,          "CUSTOM_YOLOV5_S6_REGION_GetModelMemsize error:ld is null\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == mem_tab,     "CUSTOM_YOLOV5_S6_REGION_GetModelMemsize error: NULL = mem_tab\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    memset(mem_tab, 0, sizeof(OPDEVSDK_HKA_MEM_TAB_ST) * OPDEVSDK_HKA_MEM_TAB_NUM);
    ///< calculate the memory size of model handle
    malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(sizeof(OPDEVSDK_YOLOV5_S6_REGION_MODEL_T));
    ///< call init_model function，get the information of shape
    sts = CUSTOM_YOLOV5_S6_REGION_init_model(hyperparams, param_blobs, ld, &yolov5_s6_region_model, 0, NULL);
    CHECK_ERROR(HIKFLOW_STS_OK != sts, "CUSTOM_YOLOV5_S6_REGION_GetModelMemsize error: Init Model Failed!\n", sts);
    ///< set the memory tab
    malloc_handle_tab->size = malloc_size;
    malloc_handle_tab->alignment = CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE;
    malloc_handle_tab->attrs = OPDEVSDK_HKA_MEM_PERSIST;
    malloc_handle_tab->space = OPDEVSDK_HKA_MEM_SPACE_CPU;

    return HIKFLOW_STS_OK;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_CreateModel
*  @brief      create model handle of YoloV5_s6_Region layer
*  @param[in]  const char                   *hyperparams  hyperparams of YoloV5_s6_Region layer
               const char                   *param_blobs  param_blobs of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations  of YoloV5_s6_Region layer
               OPDEVSDK_HKA_BUF_ST           mem_buf      memory resource
*  @param[out] void                        **handle       memory size and attribution needed for YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_CreateModel(const char                  *hyperparams,
                                const char                  *param_blobs,
                                OPDEVSDK_HIKFLOW_LMODEL_ST  *ld,
                                OPDEVSDK_HKA_BUF_ST          mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                                void                       **handle)
{
    int sts         = HIKFLOW_STS_OK;
    int malloc_size = 0;
    OPDEVSDK_YOLOV5_S6_REGION_MODEL_T *yolov5_s6_region_model = NULL;
    OPDEVSDK_HKA_BUF_ST    *malloc_buf = NULL;
    CHECK_ERROR(NULL == hyperparams, "CUSTOM_YOLOV5_S6_REGION_CreateModel error:hyperparams is null\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == handle, "CUSTOM_YOLOV5_S6_REGION_CreateModel error: NULL = handle\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == mem_buf, "CUSTOM_YOLOV5_S6_REGION_CreateModel error: NULL = mem_buf\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    malloc_buf = &mem_buf[0];
    ///< set npu information
    ld->npu_enum_val = OPDEVSDK_HIKFLOW_NPU_TYPE_0;
    ///< set the memory
    malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(sizeof(OPDEVSDK_YOLOV5_S6_REGION_MODEL_T));
    yolov5_s6_region_model = (OPDEVSDK_YOLOV5_S6_REGION_MODEL_T *)CUSTOM_alloc_buffer(malloc_buf,
                                                                      malloc_size,
                                                                      CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE,
                                                                      1);
    ///< call init_model function to initial the params and hyperparams
    sts = CUSTOM_YOLOV5_S6_REGION_init_model(hyperparams, param_blobs, ld, yolov5_s6_region_model, 1, malloc_buf);
    CHECK_ERROR(HIKFLOW_STS_OK != sts, "CUSTOM_YOLOV5_S6_REGION_GetModelMemsize error: Init Model Failed!\n", sts);
    *handle = yolov5_s6_region_model;
    return 0;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_GetMemsize
*  @brief      get the memory size of YoloV5_s6_Region layer for handle creation
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of YoloV5_s6_Region layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST     mem_tab  memory size and attribution needed for YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_GetMemsize(OPDEVSDK_HIKFLOW_LDATA_ST   *ld,
                               OPDEVSDK_HKA_MEM_TAB_ST      mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM])
{
    int sts              = HIKFLOW_STS_OK;
    int temp_malloc_size = 0;
    int malloc_size      = 0;
    OPDEVSDK_HKA_MEM_TAB_ST         *malloc_tab             = NULL;
    ///< assist mem tab
    OPDEVSDK_HKA_MEM_TAB_ST         *aux_tab             = NULL;
    OPDEVSDK_YOLOV5_S6_REGION_LAYER_T   yolov5_s6_region_layer    = { 0 };
    ///< import param check
    CHECK_ERROR(NULL == ld, "CUSTOM_YOLOV5_S6_REGION_GetMemsize error: NULL = ld\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == mem_tab, "CUSTOM_YOLOV5_S6_REGION_GetMemsize error: NULL = mem_tab\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    memset(mem_tab, 0, sizeof(mem_tab[0]) * OPDEVSDK_HKA_MEM_TAB_NUM);
    malloc_tab   = &mem_tab[0];
    aux_tab      = &mem_tab[1];
    yolov5_s6_region_layer.model = ld->layer_model->model_handle;
    ///< call reshape function to calculate the size of params blob
    sts = CUSTOM_YOLOV5_S6_REGION_reshape(&yolov5_s6_region_layer, ld);
    CHECK_ERROR(0 != sts,"CUSTOM_YOLOV5_S6_REGION_reshape error!", sts);
    ///< calculate the memory size which layer handle need
    temp_malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(sizeof(OPDEVSDK_YOLOV5_S6_REGION_LAYER_T));
    malloc_size     += temp_malloc_size;
    ///<  calculate the memory size which output blob need
    temp_malloc_size = ld->output_blobs[0].shape[0] *
                       ld->output_blobs[0].shape[1] *
                       ld->output_blobs[0].shape[2] *
                       ld->output_blobs[0].stride[3];
    temp_malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(temp_malloc_size);
    malloc_size     += temp_malloc_size;
    ///< calculate the scratch memory size
    temp_malloc_size = yolov5_s6_region_layer.scratch_memory_size;
    temp_malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(temp_malloc_size);
    ///< set aux memory table
    aux_tab->size      = temp_malloc_size;
    aux_tab->alignment = CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE;
    aux_tab->attrs     = OPDEVSDK_HKA_MEM_SCRATCH;
    aux_tab->space     = OPDEVSDK_HKA_MEM_SPACE_CPU;
    ///< set memory table
    malloc_tab->size      = malloc_size;
    malloc_tab->alignment = CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE;
    malloc_tab->attrs     = OPDEVSDK_HKA_MEM_PERSIST;
    malloc_tab->space     = OPDEVSDK_HKA_MEM_SPACE_CPU;

    return HIKFLOW_STS_OK;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_Create
*  @brief      create layer handle of YoloV5_s6_Region layer
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of YoloV5_s6_Region layer
               OPDEVSDK_HKA_BUF_ST          mem_buf  memory resource
*  @param[out] void                       **handle   handle of YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_Create(OPDEVSDK_HIKFLOW_LDATA_ST *ld,
                           OPDEVSDK_HKA_BUF_ST        mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                           void                     **handle)
{
    int sts              = HIKFLOW_STS_OK;
    int temp_malloc_size = 0;
    int malloc_size      = 0;

    OPDEVSDK_HKA_BUF_ST        *malloc_buf     = NULL;
    OPDEVSDK_HKA_BUF_ST        *aux_buf     = NULL;
    OPDEVSDK_YOLOV5_S6_REGION_LAYER_T  *yolov5_s6_region_layer = NULL;
    CHECK_ERROR(NULL == ld, "CUSTOM_YOLOV5_S6_REGION_Create error: NULL = ld\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == mem_buf, "CUSTOM_YOLOV5_S6_REGION_Create error: NULL = mem_buf\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == handle, "CUSTOM_YOLOV5_S6_REGION_Create error: NULL = handle\n",
                HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    malloc_buf   = &mem_buf[0];
    aux_buf      = &mem_buf[1];
    ///< allocate mem for layer handle
    malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(sizeof(OPDEVSDK_YOLOV5_S6_REGION_LAYER_T));
    yolov5_s6_region_layer = (OPDEVSDK_YOLOV5_S6_REGION_LAYER_T*) CUSTOM_alloc_buffer(malloc_buf,
                                                                      malloc_size,
                                                                      CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE,
                                                                      1);
    CHECK_ERROR(NULL == yolov5_s6_region_layer, "CUSTOM_YOLOV5_S6_REGION_Create handle CUSTOM_alloc_buffer failed!", HIKFLOW_STS_ERR_CUS_LAYER_ALLOC_ERROR);
    yolov5_s6_region_layer->model = ld->layer_model->model_handle;
    ///< call reshape function to calculate the size of params blob
    sts = CUSTOM_YOLOV5_S6_REGION_reshape(yolov5_s6_region_layer, ld);
    CHECK_ERROR(0 != sts, "CUSTOM_YOLOV5_S6_REGION_reshape error!", sts);
    ///< the output blob memory
    temp_malloc_size = ld->output_blobs[0].shape[0] *
                       ld->output_blobs[0].shape[1] *
                       ld->output_blobs[0].shape[2] *
                       ld->output_blobs[0].stride[3];
    malloc_size = CUSTOM_YOLOV5_S6_REGION_ALIGN(temp_malloc_size);
    ld->output_blobs[0].data = CUSTOM_alloc_buffer(malloc_buf,
                                                              malloc_size,
                                                              CUSTOM_YOLOV5_S6_REGION_MEM_ALIGN_SIZE,
                                                              1);
    CHECK_ERROR(NULL == ld->output_blobs[0].data, "CUSTOM_YOLOV5_S6_REGION_Create output_blobs CUSTOM_alloc_buffer failed!", HIKFLOW_STS_ERR_CUS_LAYER_ALLOC_ERROR);
    LOG_INFO("CUSTOM_MYLAYER_Create:ld->output_blobs\ndim:%d type:%d format:%d\nshpae[0]:%d shpae[1]:%d shpae[2]:%d shpae[3]:%d stride:%d\n",
             ld->output_blobs[0].dim, ld->output_blobs[0].type, ld->output_blobs[0].format,
             ld->output_blobs[0].shape[0], ld->output_blobs[0].shape[1],
             ld->output_blobs[0].shape[2], ld->output_blobs[0].shape[3],
             ld->output_blobs[0].stride[3]);
    yolov5_s6_region_layer->scratch_memory = aux_buf->cur_pos;
    *handle = yolov5_s6_region_layer;

    return HIKFLOW_STS_OK;
}

/**@fn         CUSTOM_YOLOV5_S6_REGION_Forward
*  @brief      do net forward of YoloV5_s6_Region layer
*  @param[i/o] void                       *handle   handle of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_Forward(void                      *handle,
                            OPDEVSDK_HIKFLOW_LDATA_ST *ld)
{
    int sts = HIKFLOW_STS_OK;
    void *__auxi_mem_addr__ = NULL;
 
    OPDEVSDK_YOLOV5_S6_REGION_LAYER_T  *yolov5_s6_region_layer = (OPDEVSDK_YOLOV5_S6_REGION_LAYER_T *)handle;
    __auxi_mem_addr__ = yolov5_s6_region_layer->scratch_memory;
    ///< define the input of forward function
    float *y1 = (float*)ld->input_blobs[0]->data;
    float *y2 = (float*)ld->input_blobs[1]->data;
    float *y3 = (float*)ld->input_blobs[2]->data;
    float *y4 = (float*)ld->input_blobs[3]->data;
    ///< define the output of forward function
    float *output = (float*)ld->output_blobs[0].data;
    ///< define the hyperparams of forward function
    int num_anchors = yolov5_s6_region_layer->model->num_anchors;
    int num_classes = yolov5_s6_region_layer->model->num_classes;
    float conf_thresh = yolov5_s6_region_layer->model->conf_thresh;
    float nms_thresh = yolov5_s6_region_layer->model->nms_thresh;

    ///< define the shape of forward function
    int in1_n = ld->input_blobs[0]->shape[dim4[0]];
    int in1_c = ld->input_blobs[0]->shape[dim4[1]];
    int in1_h = ld->input_blobs[0]->shape[dim4[2]];
    int in1_w = ld->input_blobs[0]->shape[dim4[3]];
    int in2_n = ld->input_blobs[1]->shape[dim4[0]];
    int in2_c = ld->input_blobs[1]->shape[dim4[1]];
    int in2_h = ld->input_blobs[1]->shape[dim4[2]];
    int in2_w = ld->input_blobs[1]->shape[dim4[3]];
    int in3_n = ld->input_blobs[2]->shape[dim4[0]];
    int in3_c = ld->input_blobs[2]->shape[dim4[1]];
    int in3_h = ld->input_blobs[2]->shape[dim4[2]];
    int in3_w = ld->input_blobs[2]->shape[dim4[3]];
    int in4_n = ld->input_blobs[3]->shape[dim4[0]];
    int in4_c = ld->input_blobs[3]->shape[dim4[1]];
    int in4_h = ld->input_blobs[3]->shape[dim4[2]];
    int in4_w = ld->input_blobs[3]->shape[dim4[3]];

    ///< define the out shape for forward function
    int outshape[4] = {0};
    CHECK_ERROR(NULL == handle, "CUSTOM_YOLOV5_S6_REGION_Forward handle = null error!\n", HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    CHECK_ERROR(NULL == ld, "CUSTOM_YOLOV5_S6_REGION_Forward ld = null error!\n", HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR);
    ///< call forward function
    sts = YoloV5_s6_Region_Forward( y1, in1_n, in1_c, in1_h, in1_w, y2, in2_n, in2_c, in2_h, in2_w, y3, in3_n, in3_c, in3_h, in3_w, y4, in4_n, in4_c, in4_h, in4_w, output, outshape, num_anchors, num_classes, conf_thresh, nms_thresh, __auxi_mem_addr__ );
    CHECK_ERROR(HIKFLOW_STS_OK != sts, "CUSTOM_YOLOV5_S6_REGION_Forward YoloV5_s6_Region_Forward failed!", HIKFLOW_STS_ERR_CUS_LAYER_FORWARD_FAILED);

    ///< set the output blob
    ld->output_blobs[0].shape[3] = outshape[dim4[3]];
    ld->output_blobs[0].shape[2] = outshape[dim4[2]];
    ld->output_blobs[0].shape[1] = outshape[dim4[1]];
    ld->output_blobs[0].shape[0] = outshape[dim4[0]];
    ld->output_blobs[0].stride[3]= OPC_SIZE_ALIGN(ld->output_blobs[0].shape[3] * sizeof(int), 16);
    
    return 0;
}