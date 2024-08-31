/***************************************************************************
 * note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right
 *Reserved.
 *
 * @file         demo_comm.c
 * @brief        realize the hikflow and media driver Demo
 *
 * @author       guanjiajun
 *
 * @date         2019/2/18
 * @version      1.0.0
 * @note         add demo for mediaDrv and hikflow
 *
 * @date         2019/6/26
 * @version      2.0.0
 * @note         Synchronized mediaDrv Interior Interface
 *
 * @date         2021/2/25
 * @version      3.0.0
 * @author       lixiaogang10
 * @note         Unified platform AND parse bin info
 *****************************************************************************/

#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>

/// 系统头文件
#include <dlfcn.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>


#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <syslog.h>


#include "opdevsdk_img.h"
#include "opdevsdk_jpegdec.h"
#include "opdevsdk_jpegenc.h"
#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_mem.h"
#include "opdevsdk_mscale.h"
#include "opdevsdk_pos.h"
#include "opdevsdk_sche.h"
#include "opdevsdk_sys.h"
#include "opdevsdk_video.h"
#include "opdevsdk_vin.h"


#include "demo_alg.h"
#include "demo_comm.h"


#include "opdevsdk_common_basic.h"

#define MODE_TCP 1
#define MODE_UDP 0

#define LISTEN_PORT 8081
#define MAX_REQ_URL_LEN 128
#define OK 0
#define ERROR -1
#define FORM_TYPE_JSON 1

#define HEOP_REG_PORT_BODY                                                     \
  "\
{\"PortMap\":\
	{\
		\"action\":\"map\",\
		\"port\":50002,\
		\"APPPort\":8081,\
		\"type\":\"TCP\"\
	}\
}\
"
static int check_weight_flag = 0; // 0表示不检测，1表示检测
static int check_weight_result = -1; // -1表示未检测，0表示不在磅上，1表示在磅上
static int check_weight_limit = 0; // 临时设置，磅的位置

#define DEMO_MAX_CHAR_NUM (256)
#define DEMO_MAX_CAL_TIMES (10 * 12.5 * 1)
#define DEMO_MAX_CAL_LOST (10 * 1000)

// static DEMO_YUV_BUF yuv_buf = {0};
static DEMO_ALG_BUF alg_buf = {0};

// static void *vir_addr = NULL;
// static void *phy_addr = 0;
static int bsche_init = 0;
static int alg_lost_times = 0;
static int check_1 = 0x11223344;
static int thread_flg = 0;
static int check_2 = 0x11223344;

static int input_width = 1920;
static int input_height = 1080;
static int alg_width = 416;
static int alg_height = 416;

static OPDEVSDK_SYS_ABILITY_ST abili;

#define MSCALE_TEST (0)
static int groupId = -1;
static void *mscale_hdl = NULL;
static int bvin_init = OP_FALSE;
static int bsys_init = OP_FALSE;

static OPDEVSDK_VIDEO_FRAME_INFO_ST vin_frame = {0};
static OPDEVSDK_VIDEO_FRAME_INFO_ST img1_frame = {0};
static OPDEVSDK_VIDEO_FRAME_INFO_ST img2_frame = {0};

unsigned long long timeStamp = 0;

#define VB_POOL_W_H_ALIGN (16) /*VB申请时计算大小时的宽高对齐*/

/*向下对齐*/
#define ALIGN_BACK(val, align) (((val) / (align)) * (align))

/*向上对齐*/
#define ALIGN_FOWARD(val, align) (ALIGN_BACK(((val) + (align) - 1), (align)))

void demo_syslog_print_do(const char *str) {
  syslog(LOG_LOCAL2 | LOG_CRIT, str);
}

void demo_syslog_print(char *format, ...) {
  char str[256] = "";
  va_list ap; /*!<可变参数结构体*/

  va_start(ap, format);
  vsnprintf(str, 255, format, ap);

  demo_syslog_print_do(str);
  va_end(ap);
}

static void demo_signal_clean() {
  // thread exit
  thread_flg = 0;

  sleep(1);

  DEMOPRT((char *)" demo_signal_clean........start !\n");

  // 异常崩溃时，需要释放opdevsdk_hikflow_Release和opdevsdk_hikflow_ReleaseModel
  // 当前两次销毁算法资源，底层会触发崩溃，暂时屏蔽算法销毁
  demo_alg_deinit();
  demo_alg_releaseBuffer();
  DEMOPRT((char *)"demo_alg_deinit\n");

  if (bsche_init) {
    bsche_init = 0;
    opdevsdk_sche_deinit();
  }

  OPDEVSDK_SYS_ABILITY_ST abili = {0};
  opdevsdk_sys_getAbility(&abili);

  int chan = abili.vinAbili.chnInfo[0].chan;

  //!!!第一步，释放已申请图像信息，否则会造成底层vb资源无法释放!!!
  if (img1_frame.timeStamp) {
    opdevsdk_mscale_releaseFrame(mscale_hdl, &img1_frame);
    // DEMOPRT((char*)"releaseFrame img1_frame frmNum %d ts %d\n",
    // img1_frame.frmNum, (int)(img1_frame.timeStamp / 1000));
  }
  if (img2_frame.timeStamp) {
    opdevsdk_mscale_releaseFrame(mscale_hdl, &img2_frame);
    // DEMOPRT((char*)"releaseFrame img2_frame frmNum %d ts %d\n",
    // img2_frame.frmNum, (int)(img2_frame.timeStamp / 1000));
  }
  if (vin_frame.timeStamp) {
    opdevsdk_vin_releaseFrame(chan, &vin_frame);
    // DEMOPRT((char*)"releaseFrame vin_frame frmNum %d ts %d\n",
    // vin_frame.frmNum, (int)(vin_frame.timeStamp / 1000));
  }

  if (mscale_hdl) {
    opdevsdk_mscale_stop(mscale_hdl);
    // DEMOPRT((char*)"opdevsdk_mscale_stop!\n");

    opdevsdk_mscale_unbindVbPoolGrp(mscale_hdl);
    // DEMOPRT((char*)"opdevsdk_mscale_unbindVbPoolGrp groupId %d mscale_hdl
    // %p!\n", groupId, mscale_hdl);

    opdevsdk_mscale_destroy(mscale_hdl);
    // DEMOPRT((char*)"opdevsdk_mscale_destroy mscale_hdl %p!\n", mscale_hdl);
    mscale_hdl = NULL;
  }

  if (-1 != groupId) {
    opdevsdk_mem_destroyVbPoolGrp(groupId);
    // DEMOPRT((char*)"opdevsdk_mem_destroyVbPoolGrp groupId %d!\n", groupId);
    groupId = -1;
  }

  if (bvin_init) {
    opdevsdk_vin_deinit(chan);
    // DEMOPRT((char*)"opdevsdk_vin_deinit chan %d!\n", chan);
    bvin_init = OP_FALSE;
  }

  // destroy the system resource
  if (bsys_init) {
    opdevsdk_sys_deinit();
    // DEMOPRT((char*)"opdevsdk_sys_deinit !\n");
    bsys_init = OP_FALSE;
  }
  // DEMOPRT((char*)" demo_signal_clean........end !\n");
}
/**
 * @brief            the function of signal acquisition
 *
 * @param[in]        none
 * @param[out] 	    none
 *
 * @return           none
 */
static void demo_signal_call_back() {
  // DEMOPRT((char*)"\n demo_main Deinit........Yes!\n");

  demo_signal_clean();
  // DEMOPRT((char*)" demo_signal_clean!\n");

  // kill the process
  system("killall opdevsdkDemo");
  // DEMOPRT((char*)"killall opdevsdkDemo");

  // exit
  exit(1);
}

/**
 * @brief             write a frame YUV to file
 *
 * @param[in]         name                                         name string
 * @param[in]         pFrm                                         YUV frame
 *
 * @return           void
 */
void demo_test_write_yuv(char *name, OPDEVSDK_VIDEO_FRAME_ST *pFrm) {

  FILE *file = NULL;
  int i;

  unsigned char *pLuma = (unsigned char *)pFrm->pVirAddr[0];
  unsigned char *pChroma = (unsigned char *)pFrm->pVirAddr[1];

  DEMO_CHECK_RET_EX(NULL == pLuma);
  DEMO_CHECK_RET_EX(NULL == pChroma);

  file = fopen(name, "wb");
  DEMO_CHECK_RET_EX(NULL == file);

  for (i = 0; i < pFrm->height; i++) {
    fwrite(pLuma + i * pFrm->width, 1, pFrm->width, file);
  }

  for (i = 0; i < pFrm->height / 2; i++) {
    fwrite(pChroma + i * pFrm->width, 1, pFrm->width, file);
  }

  fclose(file);
}

/**
 * @brief             the function of copy a frame  YUV
 *
 * @param[in]         yuvSrc                                         YUV src
 * frame
 * @param[in]         yuvDst                                         YUV dst
 * frame
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_img_copy(OPDEVSDK_VIDEO_FRAME_ST *yuvSrc,
                         OPDEVSDK_VIDEO_FRAME_ST *yuvDst) {
  int ret;

  OPDEVSDK_IMG_SCALE_PARAM_ST scale = {{{0}}};
  OPDEVSDK_IMG_RECT_PARAM_ST rect = {{0}};
  scale.rectParam.point.x = 0;
  scale.rectParam.point.y = 0;
  scale.rectParam.width = yuvSrc->width;
  scale.rectParam.height = yuvSrc->height;
  // 计算原图的中心点
  int src_center_x = yuvSrc->width / 2;
  int src_center_y = yuvSrc->height / 2;
  rect.width = yuvDst->width;
  rect.height = yuvDst->height;
  rect.point.x = src_center_x - rect.width / 2;
  rect.point.y = src_center_y - rect.height / 2;
  ret = opdevsdk_img_dmaCopy2d(yuvSrc, yuvDst, rect);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_imageScale error ret = 0x%x\n", ret);
    return ret;
  }
  // ret = opdevsdk_img_scale(yuvSrc, yuvDst, scale);
  // if (ret != OPDEVSDK_S_OK) {
  //   DEMOPRT((char *)"opdevsdk_imageScale error ret = 0x%x\n", ret);
  //   return ret;
  // }
  return OPDEVSDK_S_OK;
}

/**
 * @brief             put yuv frames to loop buffer
 *
 * @param[in]         pFrm                                         YUV frame
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_put_yuv_frame(OPDEVSDK_VIDEO_FRAME_INFO_ST *frm) {
  int ret = 0;
  OPDEVSDK_VIDEO_FRAME_INFO_ST *pfrm = NULL;
  // OPDEVSDK_VIDEO_FRAME_INFO_ST dst;
  OPDEVSDK_VIDEO_FRAME_INFO_ST *yuvDst = {0};
  if (input_width != frm->yuvFrame.width ||
      input_height != frm->yuvFrame.height) {
    alg_lost_times++;
    /* DEMOPRT((char*)"input_width %d input_height %d yuvFrame w %d h %d err\n",
       \ input_width, input_height, frm->yuvFrame.width, frm->yuvFrame.height);
     */
    return 0;
  }

  int free_buf = (alg_buf.buf_r + DEMO_MAX_ALG_BUF_COUNT - alg_buf.buf_w - 1) %
                 DEMO_MAX_ALG_BUF_COUNT;

  if (free_buf < 1) {
    alg_lost_times++;
    DEMOPRT((char *)"[ERR].alg_buf no free buf, r %d, w %d, max %d.\n",
            alg_buf.buf_r, alg_buf.buf_w, DEMO_MAX_ALG_BUF_COUNT);
    return 0;
  }

  ///< get one new frame
  pfrm = frm;
  yuvDst = &alg_buf.frm[alg_buf.buf_w];
  yuvDst->yuvFrame.width = alg_width;
  yuvDst->yuvFrame.height = alg_height;
  yuvDst->yuvFrame.pitch[0] = alg_width;
  yuvDst->yuvFrame.pitch[1] = alg_width;
  yuvDst->yuvFrame.pitch[2] = alg_width;

  yuvDst->yuvFrame.dataFormat = pfrm->yuvFrame.dataFormat;
  yuvDst->timeStamp = pfrm->timeStamp;
  yuvDst->frmNum = pfrm->frmNum;
  memcpy(&yuvDst->dateTime, &pfrm->dateTime, sizeof(OPDEVSDK_MEDIADRV_TIME_ST));
#if 0
    DEMOPRT((char*)"pfrm format %d width %d height %d pitch %d %d %d, pvir %p %p %p, pphy %p %p %p\n",
        pfrm->yuvFrame.dataFormat, pfrm->yuvFrame.width, pfrm->yuvFrame.height, pfrm->yuvFrame.pitch[0],
        pfrm->yuvFrame.pitch[1], pfrm->yuvFrame.pitch[2], pfrm->yuvFrame.pVirAddr[0], pfrm->yuvFrame.pVirAddr[1],
        pfrm->yuvFrame.pVirAddr[2], pfrm->yuvFrame.pPhyAddr[0], pfrm->yuvFrame.pPhyAddr[1], pfrm->yuvFrame.pPhyAddr[2]);
#endif
  ret = demo_img_copy(&pfrm->yuvFrame, &yuvDst->yuvFrame);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"demo_img_copy error  ret = 0x%x\n", ret);
    return 0;
  }

  alg_buf.buf_w++;
  alg_buf.buf_w = alg_buf.buf_w % DEMO_MAX_ALG_BUF_COUNT;

  return OPDEVSDK_S_OK;
}

/**
 * @brief             getting yuv frames thread
 *
 * @param[in]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
// static int demo_video_get_thread() {
void *demo_video_get_thread(void *arg) {
  int ret = 0;
  static int avg = 0, total = 0, frm_num = 0;
  // OPDEVSDK_SYS_THREAD_NAME_ST name = {"video_get"};

  usleep(40 * 1000);

  // OPDEVSDK_VIDEO_FRAME_INFO_ST frame = {0};
  OPDEVSDK_SYS_ABILITY_ST abili = {0};
  opdevsdk_sys_getAbility(&abili);
  if (abili.vinAbili.chnNum < 1) {
    DEMOPRT((char*)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
    return NULL;
  }
  int chan = abili.vinAbili.chnInfo[0].chan;
  // memset(&frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));

  while (thread_flg) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int start =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    // 阻塞获取
    ret = opdevsdk_vin_getFrame(chan, &vin_frame, -1);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_getFrame error ret = 0x%x\n", ret);
      // return -1;
    }
    // DEMOPRT((char*)"opdevsdk_vin_getFrame chan %d frm num %d ts
    // %llu\n",vin_frame.chan, vin_frame.frmNum, vin_frame.timeStamp);

    // put yuv
    ret = demo_put_yuv_frame(&vin_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"demo_put_yuv_frame  ret = 0x%x\n", ret);
    }

    gettimeofday(&tv, NULL);
    unsigned int end =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    total += (end - start);
    frm_num += 1;
    avg = total / frm_num;
    // DEMOPRT((char*)"getFrame interval %dms avg %dms\n", end - start, avg);

    ret = opdevsdk_vin_releaseFrame(chan, &vin_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_releaseFrame error ret = 0x%x\n", ret);
      // return -1;
    }
    // DEMOPRT((char*)"opdevsdk_vin_releaseFrame chan %d ts
    // %llu\n",vin_frame.chan, vin_frame.timeStamp);
    memset(&vin_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));
    // DEMOPRT((char*)"MediaSdk_getNewFrame vl %p pl %p vc %p pc %p num %d lost
    // cnt %d\n",
    // frm.yuvFrame.pVirAddr[0],frm.yuvFrame.pPhyAddr[0],frm.yuvFrame.pVirAddr[1],frm.yuvFrame.pPhyAddr[1],frm.frmNum,lost_cnt);
  }
  // DEMOPRT((char*)"###### demo_video_get_thread exit thread_flg %d \n",
  // thread_flg);

  // return ret;
}

/**
 * used in demo_init_socket_server()
 * @brief ISAPI GET
 * @param[in] char* url : url
 * @param[in] char* buf : GET content buf
 * @param[in] uint32_t bufLen : buf size
 * @return  MS_OK；others
 */
static int32_t GetMethodIsapiRetContent(char *url,
                                        OP_DEVSDK_MIME_UNIT_ST *in_buf,
                                        char *buf, unsigned int bufLen) {
  unsigned int ret = 0;
  OP_DEVSDK_DATATRANS_INPUT_ST stInput = {0};
  OP_DEVSDK_DATATRANS_OUTPUT_ST stOutput = {0};
  POP_DEVSDK_MIME_UNIT_ST out_data = NULL;
  POP_DEVSDK_MIME_UNIT_ST p_mime_st = NULL;

  if (NULL == url || strlen(url) > MAX_REQ_URL_LEN || NULL == buf ||
      0 == bufLen) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"[GetMethodIsapiRetContent] input param err!\n");
    return ERROR;
  }

  stInput.p_request_url = url;
  stInput.request_url_len = strlen(url);
  stInput.p_in_buffer = (char *)in_buf;
  stInput.in_buffer_num = 1;
  stInput.recv_time_out = 0;

  out_data =
      (POP_DEVSDK_MIME_UNIT_ST)malloc(1 * sizeof(OP_DEVSDK_MIME_UNIT_ST));
  if (NULL == out_data) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"[GetMethodIsapiRetContent] malloc fail!\n");
    ret = ERROR;
    goto EXIT1;
  }
  memset(out_data, 0, sizeof(OP_DEVSDK_MIME_UNIT_ST));
  // out_buffer_num must >0
  stOutput.out_buffer_num = 1;
  stOutput.p_out_buffer = (char *)out_data;
  out_data->p_content = buf;
  out_data->content_len = bufLen;

  if (0 != opdevsdk_dataTrans_sendDataShort(&stInput, &stOutput)) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"[GetMethodIsapiRetContent] "
                               "opdevsdk_dataTrans_sendDataShort exec fail!\n");
    ret = ERROR;
    goto EXIT2;
  }

  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"out_data->returned_size:%d\n",
                     out_data->returned_size);
  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"out_data->content_len:%d\n",
                     out_data->content_len);
  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG,
                     (char *)"stOutput.returned_buffer_num:%d\n",
                     stOutput.returned_buffer_num);
  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"out_data->p_content:\n%s\n",
                     out_data->p_content);

  p_mime_st = (POP_DEVSDK_MIME_UNIT_ST)(stOutput.p_out_buffer);

  if (NULL != p_mime_st && NULL != p_mime_st->p_content &&
      bufLen > p_mime_st->returned_size) {
    memcpy(buf, p_mime_st->p_content, p_mime_st->returned_size);
  } else {
    opdevsdk_write_log(
        OPDEVSDK_LOG_ERROR,
        (char *)"[GetMethodIsapiRetContent] p_mime_st param err!\n");
    ret = ERROR;
  }
EXIT2:
  free(out_data);
EXIT1:
  return ret;
}

/**
 * @brief            initialize the socket_server
 *
 * @param[in]        none
 * @param[out]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_init_socket_server() {
  int ret = ERROR;
  char receive_buf[1024];
  char buf_json[1024];
  char recv_buf[1024 * 4];
  int in_buf_len = 0;
  OP_DEVSDK_MIME_UNIT_ST in_data;

  (void)opdevsdk_base_init(NULL);

  /* 1. get cap */
  memset(&in_data, 0, sizeof(in_data));
  memset(recv_buf, 0, sizeof(recv_buf));

  in_data.p_content = NULL;

  ret = GetMethodIsapiRetContent(
      (char *)"GET /ISAPI/HEOP/System/capabilities?format=json", &in_data,
      recv_buf, sizeof(recv_buf));
  if (ERROR == ret) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"GetMethodIsapiRetContent error!\n");
    return ret;
  }

  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"[Listen] get cap: %s\n",
                     recv_buf);

  /* 2. register a listen port */
  memset(&in_data, 0, sizeof(in_data));
  memset(&buf_json, 0, sizeof(buf_json));
  memset(recv_buf, 0, sizeof(recv_buf));

  in_buf_len += snprintf(buf_json, sizeof(buf_json), HEOP_REG_PORT_BODY);
  in_data.data_type = FORM_TYPE_JSON;
  in_data.p_content = buf_json;
  in_data.content_len = in_buf_len;

  ret = GetMethodIsapiRetContent(
      (char *)"PUT /ISAPI/HEOP/System/APP/portMap?format=json", &in_data,
      recv_buf, sizeof(recv_buf));
  if (ERROR == ret) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"GetMethodIsapiRetContent error!\n");
    return ret;
  }

  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"[Listen] register port: %s\n",
                     recv_buf);
  sleep(1);

  /* 3. get port info */
  memset(&in_data, 0, sizeof(in_data));
  memset(recv_buf, 0, sizeof(recv_buf));
  in_data.p_content = NULL;

  ret = GetMethodIsapiRetContent(
      (char *)"GET /ISAPI/HEOP/System/APP/portMapInfo?format=json", &in_data,
      recv_buf, sizeof(recv_buf));
  if (ERROR == ret) {
    opdevsdk_write_log(OPDEVSDK_LOG_ERROR,
                       (char *)"GetMethodIsapiRetContent error!\n");
    return ret;
  }

  opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"[Listen] get port info: %s\n",
                     recv_buf);
  return 1;
}

/**
 * @brief            initialize the mediaDrv
 *
 * @param[in]        none
 * @param[out]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_init_test() {
  OPDEVSDK_SYS_VERSION_ST version = {0};

  int ret = 0;
  int i = 0;

  ///< first:initialize mediaDrv
  ret = opdevsdk_sys_init(); ///< G5
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_init error ret = 0x%x\n", ret);
    return ret;
  }

  bsys_init = OP_TRUE;

  ///< get abilities
  opdevsdk_sys_getAbility(&abili);

  // printf the abilities,mmz缓存大小在申请时由内核限制,基础库不限制,默认置0
  /* DEMOPRT((char*)"opdevsdk_init mmSize %dKB cacheSize %dKB noCacheSize %dKB,
     jpegEncAbili %d jpegDecAbili %d scaleAbili %d\n", \ abili.mmSize,
     abili.cacheSize, abili.noCacheSize, abili.jpegEncAbili, abili.jpegDecAbili,
     abili.scaleAbili);
  */

  /* for(i = 0;i < abili.vinAbili.chnNum;i++)
  // {
  //     DEMOPRT((char*)"opdevsdk_init chan_id %d w %d h %d ,fps %f\n", \
  //         abili.vinAbili.chnInfo[i].chan, abili.vinAbili.chnInfo[i].width, \
  //         abili.vinAbili.chnInfo[i].height, abili.vinAbili.chnInfo[i].fps);
  // }
  */

  ///< get version
  opdevsdk_sys_getVersion(&version);
  // DEMOPRT((char*)"opdevsdk_getVersion  version = %s\n",version.version);

  return OPDEVSDK_S_OK;
}

static int demo_mscale_prep(int *pchan, int *pgrpId, void **pms_hdl,
                            OPDEVSDK_VIN_CHN_ST *pvin_info) {

  int i, ret;
  // 开启vin
  OPDEVSDK_SYS_ABILITY_ST abili = {0};
  opdevsdk_sys_getAbility(&abili);
  if (abili.vinAbili.chnNum < 1) {
    DEMOPRT((char *)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
    return -1;
  }

  int chan = abili.vinAbili.chnInfo[0].chan;
  *pchan = chan;

  ret = opdevsdk_vin_init(chan);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_vin_init error ret = 0x%x\n", ret);
    return -1;
  }
  bvin_init = OP_TRUE;
  // DEMOPRT((char*)"opdevsdk_vin_init chan %d\n", chan);

  // 设置vi帧率
  ret = opdevsdk_vin_setFrameRate(chan, OPDEVSDK_VIDEO_FRAME_RATE_12_5);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_vin_setFrameRate error ret = 0x%x\n", ret);
    return -1;
  }

  OPDEVSDK_VIN_CHN_ST *pchn_info = &abili.vinAbili.chnInfo[0];
  memcpy(pvin_info, pchn_info, sizeof(OPDEVSDK_VIN_CHN_ST));

  // vbpool test,建议按分辨率降序输入
  //!!!注意!!!mem创建的vb pool grp要和mscale的缩放分辨率匹配，否则会绑定失败;
  // 例如，vin输入200w，mscale缩放输出4路分辨率:200w(1080p),720p;
  OPDEVSDK_MEM_VB_POOL_GRP_ST vb_pool_grp = {0};
  int tmp_w = 0, tmp_h = 0;
  vb_pool_grp.poolCnt = 2;
  snprintf(vb_pool_grp.pool[0].name, sizeof(vb_pool_grp.pool[0].name), "img1");
  vb_pool_grp.pool[0].poolId = 0; // poolId,此处建议置0
  // blkCnt对应缩放后vb缓存数量，根据需要缓存图像深度，按需申请，最少2个，建议配置4个以上(建议根据算法需要缓存个数，额外多申请2个用于mscale缩放)
  //!!!注意!!!
  //!vb缓存深度用于mscale缩放和智能算法处理，如某分辨率的vb全部被占用(智能算法get后不release)，会导致mscale缩放没有vb可用，从而影响后续缩放视频输出
  vb_pool_grp.pool[0].blkCnt = 4;
  ///!!!注意!!!vb缓存的宽高均需向上对齐操作，否则grpId绑定mscale可能会失败
  tmp_w = ALIGN_FOWARD(pchn_info->width, VB_POOL_W_H_ALIGN);  // 1920
  tmp_h = ALIGN_FOWARD(pchn_info->height, VB_POOL_W_H_ALIGN); // 1080
  vb_pool_grp.pool[0].blkSize = tmp_w * tmp_h * 3 / 2;
  snprintf(vb_pool_grp.pool[1].name, sizeof(vb_pool_grp.pool[1].name), "img2");
  vb_pool_grp.pool[1].poolId = 0;
  vb_pool_grp.pool[1].blkCnt = 4;
  tmp_w = ALIGN_FOWARD(alg_width, VB_POOL_W_H_ALIGN);  // 1280
  tmp_h = ALIGN_FOWARD(alg_height, VB_POOL_W_H_ALIGN); // 720
  vb_pool_grp.pool[1].blkSize = tmp_w * tmp_h * 3 / 2;

  int grpId = 0;
  grpId = opdevsdk_mem_createVbPoolGrp(&vb_pool_grp);
  if (grpId < 0) {
    ret = grpId;
    DEMOPRT((char *)"opdevsdk_mem_createVbPoolGrp error ret = 0x%x\n", ret);
    return ret;
  }
  for (i = 0; i < vb_pool_grp.poolCnt; i++) {
    DEMOPRT(
        (char *)"vbpool create idx %d name %s poolId %d blkCnt %d blkSize %d\n",
        i, vb_pool_grp.pool[i].name, vb_pool_grp.pool[i].poolId,
        vb_pool_grp.pool[i].blkCnt, vb_pool_grp.pool[i].blkSize);
  }
  *pgrpId = grpId;

  void *ms_hdl = NULL;
  OPDEVSDK_MSCALE_INIT_ST mscale_info = {0};
  mscale_info.srcImg.width = pchn_info->width;
  mscale_info.srcImg.height = pchn_info->height;
  mscale_info.srcImg.pitch = pchn_info->width;
  mscale_info.dstImgCnt = 2;
  mscale_info.dstImg[0].width = pchn_info->width;
  mscale_info.dstImg[0].height = pchn_info->height;
  mscale_info.dstImg[0].pitch = pchn_info->width;
  mscale_info.dstImg[1].width = alg_width;   // 1280;
  mscale_info.dstImg[1].height = alg_height; // 720;
  mscale_info.dstImg[1].pitch = alg_width;   // 1280;
  ret = opdevsdk_mscale_create(&ms_hdl, &mscale_info);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_mscale_create error ret = 0x%x\n", ret);
    return ret;
  }
  *pms_hdl = ms_hdl;
  DEMOPRT((char *)"opdevsdk_mscale_create ok\n");

  // grp id 绑定,grpid对应的vb pool信息需要和mscale_hdl匹配
  ret = opdevsdk_mscale_bindVbPoolGrp(ms_hdl, grpId);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_mscale_create error ret = 0x%x\n", ret);
    return ret;
  }
  // DEMOPRT((char*)"opdevsdk_mscale_bindVbPoolGrp grpId %d\n", grpId);

  ret = opdevsdk_mscale_start(ms_hdl);
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"opdevsdk_mscale_start error ret = 0x%x\n", ret);
    return ret;
  }
  // DEMOPRT((char*)"opdevsdk_mscale_start \n");
  return 0;
}

/**
 * @brief            initialize algorithm scheduler
 *
 * @param[in]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_scheduler_test() {
  int ret = 0;
  // char patch[] = "/heop/package/opdevsdkDemo/hik_bin";

  ///< scheduler init
  ret = opdevsdk_sche_init();
  if (ret != OPDEVSDK_SCHE_S_OK) {
    DEMOPRT((char *)"opdevsdk_sche_init error ret = 0x%x\n", ret);
    return ret;
  }
  bsche_init = 1;
  DEMOPRT((char *)"opdevsdk_sche_init\n");

  ///< HIKFlow version
  OPDEVSDK_HKA_VERSION_ST version;
  opdevsdk_hikflow_GetVersion(&version);
  // DEMOPRT((char*)"opdevsdk_hikflow_GetVersion: %s\n", version.version);

  return OPDEVSDK_SCHE_S_OK;
}

/**
 * @brief            initialize algorithm buffer
 *
 * @param[in]         pyuv_buf                                         YUV frame
 * buffer struct
 * @param[in]         width                                           image
 * width
 * @param[in]         height                                          image
 * height
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_alg_buf_init(DEMO_ALG_BUF *pyuv_buf, int width, int height) {
  int i, ret;
  void *phy_addr_buf = NULL;

  for (i = 0; i < DEMO_MAX_ALG_BUF_COUNT; i++) {
    ret = opdevsdk_mem_allocCache(&phy_addr_buf,
                                  &pyuv_buf->frm[i].yuvFrame.pVirAddr[0], "ALG",
                                  width * height * 1.5);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_mem_allocCache 1 error ret = 0x%x\n", ret);
      return ret;
    }

    pyuv_buf->frm[i].yuvFrame.pPhyAddr[0] = (void *)(PTR_VOID)phy_addr_buf;
    pyuv_buf->frm[i].yuvFrame.pVirAddr[1] =
        pyuv_buf->frm[i].yuvFrame.pVirAddr[0] + width * height;
    pyuv_buf->frm[i].yuvFrame.pPhyAddr[1] =
        (void *)(PTR_VOID)phy_addr_buf + width * height;
    pyuv_buf->frm[i].yuvFrame.pVirAddr[2] =
        pyuv_buf->frm[i].yuvFrame.pVirAddr[0] + width * height;
    pyuv_buf->frm[i].yuvFrame.pPhyAddr[2] =
        (void *)(PTR_VOID)phy_addr_buf + width * height;
    // DEMOPRT((char*)"pyuv_buf->frm[i].yuvFrame.pPhyAddr[0] %p
    // %p\n",pyuv_buf->frm[i].yuvFrame.pPhyAddr[0],pyuv_buf->frm[i].yuvFrame.pVirAddr[0]);
  }

  pyuv_buf->buf_size = width * height * 3 / 2;
  pyuv_buf->buf_w = 0;
  pyuv_buf->buf_r = 0;

  return OPDEVSDK_S_OK;
}

/**
 * @brief            initialize yuv buffer
 *
 * @param[in] 		pyuv_buf
 * YUV frame buffer struct
 * @param[in] 	    width                                           image width
 * @param[in] 	    height                                          image height
 *
 * @return           0 if successful, otherwise an error number returned
 */

static int demo_yuv_buf_init(DEMO_YUV_BUF *pyuv_buf, int width, int height) {
  int i, ret;
  void *phy_addr_buf = NULL;

  for (i = 0; i < DEMO_MAX_BUF_COUNT; i++) {
    ret = opdevsdk_mem_allocCache(&phy_addr_buf,
                                  &pyuv_buf->frm[i].yuvFrame.pVirAddr[0], "YUV",
                                  width * height * 1.5);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_mem_allocCache 1 error ret = 0x%x\n", ret);
      return ret;
    }

    pyuv_buf->frm[i].yuvFrame.pPhyAddr[0] = (void *)(PTR_VOID)phy_addr_buf;
    pyuv_buf->frm[i].yuvFrame.pVirAddr[1] =
        pyuv_buf->frm[i].yuvFrame.pVirAddr[0] + width * height;
    pyuv_buf->frm[i].yuvFrame.pPhyAddr[1] =
        (void *)(PTR_VOID)phy_addr_buf + width * height;
    pyuv_buf->frm[i].yuvFrame.pVirAddr[2] =
        pyuv_buf->frm[i].yuvFrame.pVirAddr[0] + width * height;
    pyuv_buf->frm[i].yuvFrame.pPhyAddr[2] =
        (void *)(PTR_VOID)phy_addr_buf + width * height;
    // DEMOPRT((char*)"pyuv_buf->frm[i].yuvFrame.pPhyAddr[0] %p
    // %p\n",pyuv_buf->frm[i].yuvFrame.pPhyAddr[0],pyuv_buf->frm[i].yuvFrame.pVirAddr[0]);
  }

  pyuv_buf->buf_size = width * height * 3 / 2;
  pyuv_buf->buf_w = 0;
  pyuv_buf->buf_r = 0;

  return OPDEVSDK_S_OK;
}

/**
 * @brief            one algorithm processing thread with getting yuv from file
 *
 * @param[in]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
// static int demo_alg_proc_from_file_thread()
void *demo_alg_proc_from_file_thread(void *arg) {
  int ret = 0;
  ret = demo_alg_proc_fromFile();
  if (ret != OPDEVSDK_S_OK) {
    DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
  }
  return OPDEVSDK_S_OK;
}

/**
 * @brief 			the function of copy a frame  YUV
 *
 * @param[in] 		yuvSrc
 * YUV src frame
 * @param[in] 		yuvDst
 * YUV dst frame
 *
 * @return           0 if successful, otherwise an error number returned
 */
static int demo_alg_proc(OPDEVSDK_VIDEO_FRAME_INFO_ST *pfrm,
                         OPDEVSDK_POS_TARGET_ST *ptarget, int num,
                         OPDEVSDK_POS_ALERT_INFO_ST *palarm) {
  int i = 0;
  int offset = num % 400;

  for (i = 0; i < 8; i++) {
    ptarget[i].id = i + 1;
    ptarget[i].trace_time = 0; // 0-目标框,1-目标轨迹
    ptarget[i].color = 0;
    ptarget[i].region.pointNum = 4;
    ptarget[i].region.point[0].x = 0.1 + i * 0.1;
    ptarget[i].region.point[0].y = 0.1 + offset * 0.002;
    ptarget[i].region.point[1].x = 0.1 + i * 0.1;
    ptarget[i].region.point[1].y = 0.2 + offset * 0.002;
    ptarget[i].region.point[2].x = 0.2 + i * 0.1;
    ptarget[i].region.point[2].y = 0.2 + offset * 0.002;
    ptarget[i].region.point[3].x = 0.2 + i * 0.1;
    ptarget[i].region.point[3].y = 0.1 + offset * 0.002;
  }
  if (num % 100 == 0) {
    static int idx = 0;
    palarm->target.id = ptarget[idx].id;
    palarm->target.region.pointNum = 4;
    ;
    palarm->target.region.point[0].x = ptarget[idx].region.point[0].x;
    palarm->target.region.point[0].y = ptarget[idx].region.point[0].y;
    palarm->target.region.point[1].x = ptarget[idx].region.point[1].x;
    palarm->target.region.point[1].y = ptarget[idx].region.point[1].y;
    palarm->target.region.point[2].x = ptarget[idx].region.point[2].x;
    palarm->target.region.point[2].y = ptarget[idx].region.point[2].y;
    palarm->target.region.point[3].x = ptarget[idx].region.point[3].x;
    palarm->target.region.point[3].y = ptarget[idx].region.point[3].y;
    idx++;
    idx %= 8;
  }
#if 0
    char yuvName[DEMO_MAX_CHAR_NUM]; 
    static int idx = 0;
    sprintf(yuvName,"./yuv_%d_%d_%d_from_1920_1080_scale.yuv",
    pfrm->yuvFrame.width,pfrm->yuvFrame.height,idx);
    demo_test_write_yuv(yuvName,&pfrm->yuvFrame);
#endif

  return OPDEVSDK_S_OK;
}

/**
 * @brief            one algorithm processing thread with sending target list to
 * WEB for displaying
 *
 * @param[in] 		none
 *
 * @return           0 if successful, otherwise an error number returned
 */
// static int demo_alg_proc_thread() {
void *demo_alg_proc_thread(void *arg) {
  int ret = 0;
  OPDEVSDK_VIDEO_FRAME_INFO_ST *pfrm = NULL;

  static int avg = 0, total = 0, frm_num = 0;

  OPDEVSDK_POS_TARGET_ST target[MAX_OUTPUT_BOX_NUM] = {0};
  OPDEVSDK_POS_ALERT_INFO_ST alarm = {OPDEVSDK_POS_TIME_TYPE_1K};
  OPDEVSDK_SYS_ABILITY_ST abili = {OPDEVSDK_POS_TIME_TYPE_1K};
  opdevsdk_sys_getAbility(&abili);
  if (abili.vinAbili.chnNum < 1) {
    DEMOPRT((char*)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
    return NULL;
  }
  int chan = abili.vinAbili.chnInfo[0].chan;

  int alarm_frm = 0;
  ;
  OPDEVSDK_POS_TARGET_LIST_INFO_ST pack_target = {OPDEVSDK_POS_TIME_TYPE_1K};
  while (thread_flg) {

    if (alg_buf.buf_w == alg_buf.buf_r) {
      usleep(10 * 1000);
      continue;
    }

    ///< get one frame yuv
    pfrm = &alg_buf.frm[alg_buf.buf_r];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int start =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    // algorithm processing and return target list
    memset(&pack_target, 0, sizeof(pack_target));
    memset(&target[0], 0, sizeof(target));
    pack_target.tgtList.pTgt = target;
    int check_flag = 0;

    ret = demo_alg_proc_fromCamera(pfrm, &pack_target, check_weight_limit);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
    }
    gettimeofday(&tv, NULL);
    unsigned int end =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    alg_buf.buf_r++;
    alg_buf.buf_r = alg_buf.buf_r % DEMO_MAX_ALG_BUF_COUNT;

    // DEMOPRT((char*)"put target frm %d, ts %llu\n", pfrm->frmNum,
    // pfrm->timeStamp);

    // 模拟算法报警,30帧间隔
    memset(&alarm, 0, sizeof(alarm));
    if (frm_num - alarm_frm >= 30) {
      if (pack_target.tgtList.tgtNum) {
        alarm.target.id = target[0].id;
        alarm.target.region.pointNum = 4;
        ;
        alarm.target.region.point[0].x = target[0].region.point[0].x;
        alarm.target.region.point[0].y = target[0].region.point[0].y;
        alarm.target.region.point[1].x = target[0].region.point[1].x;
        alarm.target.region.point[1].y = target[0].region.point[1].y;
        alarm.target.region.point[2].x = target[0].region.point[2].x;
        alarm.target.region.point[2].y = target[0].region.point[2].y;
        alarm.target.region.point[3].x = target[0].region.point[3].x;
        alarm.target.region.point[3].y = target[0].region.point[3].y;
        alarm_frm = frm_num;
      }
    }

    // 目标打包
    pack_target.timeType = OPDEVSDK_POS_TIME_TYPE_1K;
    pack_target.packId = 0;                         // 预留
    pack_target.timeStamp = pfrm->timeStamp / 1000; // 时间戳填写实际yuv时间戳
    pack_target.attribute = OPDEVSDK_POS_POLYGON_ATTRI_NORMAL;
    pack_target.tgtList.tgtNum = 8;
    pack_target.tgtList.pTgt = &target[0];
    // send data to WEB for displaying
    ret = opdevsdk_pos_procTarget(chan, &pack_target);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_videoDemoVca error  ret = 0x%x\n", ret);
    }

    // 规则打包
    OPDEVSDK_POS_RULE_ST rule_info[2] = {{0}};
    rule_info[0].id = 1;
    rule_info[0].enable = 1;
    rule_info[0].ruleType = OPDEVSDK_POS_RULE_TYPE_LINE;
    rule_info[0].polygon.pointNum = 2;
    rule_info[0].polygon.point[0].x = 0.2;
    rule_info[0].polygon.point[0].y = 0.2;
    rule_info[0].polygon.point[1].x = 0.8;
    rule_info[0].polygon.point[1].y = 0.8;
    rule_info[1].id = 2;
    rule_info[1].enable = 1;
    rule_info[1].ruleType = OPDEVSDK_POS_RULE_TYPE_REGION;
    rule_info[1].polygon.pointNum = 4;
    rule_info[1].polygon.point[0].x = 0.2;
    rule_info[1].polygon.point[0].y = 0.2;
    rule_info[1].polygon.point[1].x = 0.2;
    rule_info[1].polygon.point[1].y = 0.8;
    rule_info[1].polygon.point[2].x = 0.8;
    rule_info[1].polygon.point[2].y = 0.8;
    rule_info[1].polygon.point[3].x = 0.8;
    rule_info[1].polygon.point[3].y = 0.2;
    OPDEVSDK_POS_RULE_LIST_INFO_ST rule = {OPDEVSDK_POS_TIME_TYPE_1K};
    rule.timeType = OPDEVSDK_POS_TIME_TYPE_1K;
    rule.timeStamp = pfrm->timeStamp / 1000; // 时间戳填写实际yuv时间戳
    rule.attribute = OPDEVSDK_POS_POLYGON_ATTRI_NORMAL;
    rule.ruleList.ruleNum = 2;
    rule.ruleList.pRule = &rule_info[0];
    ret = opdevsdk_pos_procRule(chan, &rule);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_pos_procRule error ret = 0x%x\n", ret);
      // return -1;
    }

    // 报警目标
    if (frm_num % 300 == 0) {
      static int idx = 0;
      alarm.ruleInfo.id = rule_info[idx].id;
      alarm.ruleInfo.enable = 1;
      alarm.ruleInfo.ruleType = rule_info[idx].ruleType;
      alarm.ruleInfo.polygon.pointNum = rule_info[idx].polygon.pointNum;
      unsigned int i;
      for (i = 0; i < rule_info[idx].polygon.pointNum; i++) {
        alarm.ruleInfo.polygon.point[i].x = rule_info[idx].polygon.point[i].x;
        alarm.ruleInfo.polygon.point[i].y = rule_info[idx].polygon.point[i].y;
      }
      idx++;
      idx %= 2;
      ret = opdevsdk_pos_procAlarm(chan, &alarm);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"opdevsdk_pos_procAlarm error ret = 0x%x\n", ret);
        // return -1;
      }
    }

    // calculate
    // DEMOPRT((char*)"demo_alg_proc costtime:%dms \n",end - start);
    total += (end - start);
    frm_num += 1;
    avg = total / frm_num;
    // DEMOPRT((char*)"alg proc interval %dms avg %dms\n", end - start, avg);
  }
  // DEMOPRT((char*)"###### alg_proc_thrd exit thread_flg %d \n", thread_flg);

  return OPDEVSDK_S_OK;
}

/**
 * @brief 			getting yuv frames thread
 *
 * @param[in] 		none
 *
 * @return           0 if successful, otherwise an error number returned
 */
void *demo_vin_get_thread(void *arg)
// static int demo_vin_get_thread(void *arg)
{
  if (arg != mscale_hdl) {
    DEMOPRT((char *)"arg %p != mscale_hdl %p err\n", arg, mscale_hdl);
    // return -1;
    return NULL;
  }
  int ret = 0;
  static int avg = 0, total = 0, frm_num = 0;
  // OPDEVSDK_SYS_THREAD_NAME_ST name = {"video_get"};

  int yuv_lost_times = 0;
  // h7平台，双核cpu，智能优先跑第2核
  // devsdk_pthrd_set_thread_bind_core(1);
  // devsdk_pthrd_set_name("vin_get_thrd");

  usleep(40 * 1000);

  // OPDEVSDK_VIDEO_FRAME_INFO_ST frame = {0};
  OPDEVSDK_SYS_ABILITY_ST abili = {0};
  opdevsdk_sys_getAbility(&abili);
  if (abili.vinAbili.chnNum < 1) {
    DEMOPRT((char *)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
    // return -1;
    return NULL;
  }
  int chan = abili.vinAbili.chnInfo[0].chan;
  // memset(&frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));

  while (thread_flg) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int start =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    // 阻塞获取
    ret = opdevsdk_vin_getFrame(chan, &vin_frame, -1);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_getFrame error ret = 0x%x\n", ret);
      yuv_lost_times++;
      // return -1;
    }
    // if(frm_num % 25 == 0)
    // {
    //     DEMOPRT((char*)"opdevsdk_vin_getFrame chan %d frm num %d ts %llu
    //     %02d:%02d:%02d_%03d\n",
    //         vin_frame.chan, vin_frame.frmNum, vin_frame.timeStamp,
    //         vin_frame.dateTime.hour, vin_frame.dateTime.minute,
    //         vin_frame.dateTime.second, vin_frame.dateTime.milliSecond);
    // }
    timeStamp = vin_frame.timeStamp;

    // opdevsdk_mscale_sendFrame必须使用vb缓存,否则可能send失败
    ret = opdevsdk_mscale_sendFrame(mscale_hdl, &vin_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_mscale_sendFrame  ret = 0x%x\n", ret);
      yuv_lost_times++;
    }

    ret = opdevsdk_vin_releaseFrame(chan, &vin_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_releaseFrame error ret = 0x%x\n", ret);
      yuv_lost_times++;
      // return -1;
    }
    if (frm_num % 25 == 0) {
      DEMOPRT((char *)"opdevsdk_vin_releaseFrame chan %d ts %llu\n",
              vin_frame.chan, vin_frame.timeStamp);
    }
    memset(&vin_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));

    gettimeofday(&tv, NULL);
    unsigned int end =
        (((unsigned int)tv.tv_sec) * 1000 + ((unsigned int)tv.tv_usec) / 1000);

    total += (end - start);
    frm_num += 1;
    avg = total / frm_num;
    if (frm_num % 25 == 0) {
      DEMOPRT((char *)"getFrame interval %dms avg %dms\n", end - start, avg);
    }
  }
  DEMOPRT((char *)"###### vin_get_thrd exit thread_flg %d check_1 0x%x check_2 "
                  "0x%x\n",
          thread_flg, check_1, check_2);

  // return ret;
  return NULL;
}

/**
 * @brief            one algorithm processing thread with sending target list to
 * WEB for displaying
 *
 * @param[in] 		none
 *
 * @return           0 if successful, otherwise an error number returned
 */
void *demo_alg_proc_thread_2(void *arg)
// static int demo_alg_proc_thread_2()
{
  int ret = 0;
  // OPDEVSDK_SYS_THREAD_NAME_ST name = {"alg_proc"};

  static int avg = 0, total = 0, frm_num = 0;

  OPDEVSDK_POS_TARGET_ST target[MAX_OUTPUT_BOX_NUM] = {0};
  OPDEVSDK_SYS_ABILITY_ST abili = {0};
  opdevsdk_sys_getAbility(&abili);
  if (abili.vinAbili.chnNum < 1) {
    // DEMOPRT((char*)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
    // return -1;
    return NULL;
  }
  int chan = abili.vinAbili.chnInfo[0].chan;

  // OPDEVSDK_VIDEO_FRAME_INFO_ST frame1 = {0};
  // OPDEVSDK_VIDEO_FRAME_INFO_ST frame2 = {0};
  OPDEVSDK_MSCALE_RES_ST img1 = {0};
  OPDEVSDK_MSCALE_RES_ST img2 = {0};
  img1.width = input_width;
  img1.height = input_height;
  img1.pitch = input_width;
  img2.width = alg_width;
  img2.height = alg_height;
  img2.pitch = alg_width;

  int mscale_get_err = 0;
  int mscale_rel_err = 0;

  OPDEVSDK_POS_TARGET_LIST_INFO_ST pack_target = {OPDEVSDK_POS_TIME_TYPE_1K};

  while (thread_flg) {
    DEMOPRT((char*)"demo_alg_process: camera!");
    // 阻塞get
    ret = opdevsdk_mscale_getFrame(mscale_hdl, img1, &img1_frame, -1);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
      mscale_get_err++;
    }
    // img2 get，也可单独开一个线程获取
    ret = opdevsdk_mscale_getFrame(mscale_hdl, img2, &img2_frame, -1);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
      mscale_get_err++;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int start = (((unsigned int)tv.tv_sec) * 1000 +
                          ((unsigned int)tv.tv_usec) / 1000);

    // algorithm processing and return target list
    memset(&pack_target, 0, sizeof(pack_target));
    memset(&target[0], 0, sizeof(target));
    pack_target.tgtList.pTgt = &target[0];
    int check_weight_result = -1;
    check_weight_result = demo_alg_proc_fromCamera(&img2_frame, &pack_target,
                                                   check_weight_limit);
    DEMOPRT((char *)"*********check_weight_result:%d*******\n",
            check_weight_result);
    gettimeofday(&tv, NULL);
    unsigned int end = (((unsigned int)tv.tv_sec) * 1000 +
                        ((unsigned int)tv.tv_usec) / 1000);

    alg_buf.buf_r++;
    alg_buf.buf_r = alg_buf.buf_r % DEMO_MAX_ALG_BUF_COUNT;
    if (frm_num % 25 == 0) {
      DEMOPRT((char *)"put target frm %d, ts %llu\n", img1_frame.frmNum,
              img1_frame.timeStamp);
    }

    // 目标打包
    pack_target.timeType = OPDEVSDK_POS_TIME_TYPE_1K;
    pack_target.packId = 0; // 预留
    pack_target.timeStamp =
        timeStamp /
        1000; // img1_frame.timeStamp /
              // 1000;//hikflow算法处理耗时过大，时间戳取最新时间戳
    pack_target.attribute = OPDEVSDK_POS_POLYGON_ATTRI_NORMAL;
    // pack_target.tgtList.tgtNum  = 8;
    pack_target.tgtList.pTgt = &target[0];
    // send data to WEB for displaying
    ret = opdevsdk_pos_procTarget(chan, &pack_target);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_videoDemoVca error  ret = 0x%x\n", ret);
    }

    // img1 release
    ret = opdevsdk_mscale_releaseFrame(mscale_hdl, &img1_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_mscale_releaseFrame error  ret = 0x%x\n",
              ret);
      mscale_rel_err++;
    }
    memset(&img1_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));
    // img2 release
    ret = opdevsdk_mscale_releaseFrame(mscale_hdl, &img2_frame);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_mscale_releaseFrame error  ret = 0x%x\n",
              ret);
      mscale_rel_err++;
    }
    memset(&img2_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));
    // calculate
    DEMOPRT((char*)"demo_alg_proc costtime:%dms \n",end - start);
    total += (end - start);
    frm_num += 1;
    avg = total / frm_num;
    if(frm_num % 25 == 0)
    {
        DEMOPRT((char*)"alg proc interval %dms avg %dms\n", end - start,
        avg);
    }

    if (check_weight_flag == 1) {
      // 阻塞get
      ret = opdevsdk_mscale_getFrame(mscale_hdl, img1, &img1_frame, -1);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
        mscale_get_err++;
      }
      // img2 get，也可单独开一个线程获取
      ret = opdevsdk_mscale_getFrame(mscale_hdl, img2, &img2_frame, -1);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"demo_alg_process error  ret = 0x%x\n", ret);
        mscale_get_err++;
      }

      struct timeval tv;
      gettimeofday(&tv, NULL);
      unsigned int start = (((unsigned int)tv.tv_sec) * 1000 +
                            ((unsigned int)tv.tv_usec) / 1000);

      // algorithm processing and return target list
      memset(&pack_target, 0, sizeof(pack_target));
      memset(&target[0], 0, sizeof(target));
      pack_target.tgtList.pTgt = &target[0];
      int check_weight_result = -1;
      check_weight_result = demo_alg_proc_fromCamera(&img2_frame, &pack_target,
                                                     check_weight_limit);
      DEMOPRT((char *)"*********check_weight_result:%d*******\n",
              check_weight_result);
      gettimeofday(&tv, NULL);
      unsigned int end = (((unsigned int)tv.tv_sec) * 1000 +
                          ((unsigned int)tv.tv_usec) / 1000);

      alg_buf.buf_r++;
      alg_buf.buf_r = alg_buf.buf_r % DEMO_MAX_ALG_BUF_COUNT;
      if (frm_num % 25 == 0) {
        DEMOPRT((char *)"put target frm %d, ts %llu\n", img1_frame.frmNum,
                img1_frame.timeStamp);
      }

      // 目标打包
      pack_target.timeType = OPDEVSDK_POS_TIME_TYPE_1K;
      pack_target.packId = 0; // 预留
      pack_target.timeStamp =
          timeStamp /
          1000; // img1_frame.timeStamp /
                // 1000;//hikflow算法处理耗时过大，时间戳取最新时间戳
      pack_target.attribute = OPDEVSDK_POS_POLYGON_ATTRI_NORMAL;
      // pack_target.tgtList.tgtNum  = 8;
      pack_target.tgtList.pTgt = &target[0];
      // send data to WEB for displaying
      ret = opdevsdk_pos_procTarget(chan, &pack_target);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"opdevsdk_videoDemoVca error  ret = 0x%x\n", ret);
      }

      // img1 release
      ret = opdevsdk_mscale_releaseFrame(mscale_hdl, &img1_frame);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"opdevsdk_mscale_releaseFrame error  ret = 0x%x\n",
                ret);
        mscale_rel_err++;
      }
      memset(&img1_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));
      // img2 release
      ret = opdevsdk_mscale_releaseFrame(mscale_hdl, &img2_frame);
      if (ret != OPDEVSDK_S_OK) {
        DEMOPRT((char *)"opdevsdk_mscale_releaseFrame error  ret = 0x%x\n",
                ret);
        mscale_rel_err++;
      }
      memset(&img2_frame, 0, sizeof(OPDEVSDK_VIDEO_FRAME_INFO_ST));
      // calculate
      // DEMOPRT((char*)"demo_alg_proc costtime:%dms \n",end - start);
      total += (end - start);
      frm_num += 1;
      avg = total / frm_num;
      // if(frm_num % 25 == 0)
      // {
      //     DEMOPRT((char*)"alg proc interval %dms avg %dms\n", end - start,
      //     avg);
      // }
      check_weight_flag = 0;
    } else {
      sleep(1);
    }
    // DEMOPRT((char*)"###### alg_proc_thrd exit thread_flg %d check_1 0x%x
    // check_2 0x%x\n", thread_flg, check_1, check_2);

    // return OPDEVSDK_S_OK;
  }
  return OPDEVSDK_S_OK;
}

/**
 * @brief            socket connection from client
 *
 * @param[in]         none
 *
 * @return           0 if successful, otherwise an error number returned
 */
void *demo_socket_proc(void *arg)
// static void demo_socket_proc()
{
  // make sure dem_init_socker_server have run
  // socket connection
  char recv_buf[1024 * 4];
  char receive_buf[1024];
  int ret = ERROR;

  struct sockaddr_in tcp_server_addr, tcp_client_addr;
  int socket_fd, connect_sock;
  int opt;
  // int length = 0;
  socklen_t length = 0;
  struct in_addr addrptr;
  length = sizeof(struct sockaddr);

  memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));

  tcp_server_addr.sin_family = AF_INET;
  tcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcp_server_addr.sin_port = htons(LISTEN_PORT);

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"socket create error\n");
  } else {
    opt = SO_REUSEADDR;
    (void)setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(socket_fd, (struct sockaddr *)&tcp_server_addr,
             sizeof(tcp_server_addr)) == -1) {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"bind error! \n");
    }

    if (listen(socket_fd, 3)) {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"Server listen error! \n");
    }
  }

  while (1) {
    connect_sock =
        accept(socket_fd, (struct sockaddr *)&tcp_client_addr, &length);
    if (connect_sock < 0) {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"accept error!\n");
      sleep(3);
      continue;
    } else {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"success link! \n");
    }

    memset(receive_buf, 0, sizeof(receive_buf));
    ret = read(connect_sock, receive_buf, sizeof(receive_buf));
    if (ret > 0) {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG, (char *)"@@@server recv: %s@@@\n",
                         receive_buf);
      // 判断是否是传参数的
      if ((NULL != strstr(receive_buf, "/config?weight_limit="))) {
        char *p = strstr(receive_buf, "/config?weight_limit=");
        char *p2 = strstr(receive_buf, "end");
        *p2 = '\0';
        p += strlen("/config?weight_limit=");
        int check_weight_limit = atoi(p);
        DEMOPRT((char *)"设置地磅参数为：%d", check_weight_limit);
      } else // 不传参数，判断车辆是否在磅上
      {
        check_weight_flag = 1;
      }
    }

    sleep(0.5); // 确保另一个进程有响应，检查check_weight_flag =
                // 1时给出ck_weight_result

    // char *json_string = "HTTP/1.1 200 OK \n\r Content-Type:
    // application/json;charset=UTF-8 \n\r "; char *s2 = "[{'check':1}]\n\r";
    // strncat(json_string, s2, strlen(s2)+1);
    char json_string[1024] = "HTTP/1.1 200 OK \n\r";
    if (check_weight_result == 0) {
      const char *s2 = " {\"check\":0}\n\r";
      strncat(json_string, s2, strlen(s2));
    } else if (check_weight_result == 1) {
      const char *s2 = " {\"check\":1}\n\r";
      strncat(json_string, s2, strlen(s2));
    } else {
      const char *s2 = " {\"check\":-1}\n\r";
      strncat(json_string, s2, strlen(s2));
    }
    ret = write(connect_sock, json_string, strlen(json_string));
    if (ret < 0) {
      opdevsdk_write_log(OPDEVSDK_LOG_DEBUG,
                         (char *)"server write err!  ret === %d\n", ret);
    }

    close(connect_sock);
    connect_sock = -1;
    check_weight_flag = 0;
  }
}

/**
 * @brief            开启core dump
 *
 * @param[in]        pCorePid        PID
 * @param[in]        pCorePath       保存路径
 *
 * @param[out]       none
 *
 * @return           0 if successful, otherwise an error number returned
 */
int demo_core_dump_open(char *pCorePid, char *pCorePath) {
  int iRet = -1;
  int iFd1 = -1;
  int iFd2 = -1;
  struct rlimit limit;
  struct rlimit limit_set;

  do {
    /// 1, set core ulimit
    if (getrlimit(RLIMIT_CORE, &limit)) {
      DEMOPRT((char *)"get resource limit fail!\n");
      break;
    }

    limit_set.rlim_cur = limit_set.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &limit_set)) {
      limit_set.rlim_cur = limit_set.rlim_max = limit.rlim_max;
      if (limit.rlim_max != RLIM_INFINITY) {
        // IPP_ERR( "CORE: cur=0x%x, max=0x%x\n",
        // limit.rlim_cur, limit.rlim_max);
      }

      if (setrlimit(RLIMIT_CORE, &limit_set)) {
        DEMOPRT((char *)"set core unlimited fail!\n");
        break;
      }
    }

    /// 2, set core use pid
    if (pCorePid && strlen(pCorePid) > 0) {
      iFd1 = open("/proc/sys/kernel/core_uses_pid", O_RDWR | O_NDELAY | O_TRUNC,
                  0644);
      if (iFd1 < 0) {
        DEMOPRT((char *)"open core_uses_pid fail!\n");
        break;
      }
      if (strlen(pCorePid) != write(iFd1, pCorePid, strlen(pCorePid))) {
        DEMOPRT((char *)"set core_uses_pid fail!\n");
        close(iFd1);
        break;
      }
      close(iFd1);
    }

    /// 3, set core pattern
    if (pCorePath && strlen(pCorePath) > 0) {
      iFd2 = open("/proc/sys/kernel/core_pattern", O_RDWR | O_NDELAY | O_TRUNC,
                  0644);
      if (iFd2 < 0) {
        DEMOPRT((char *)"open core_pattern fail!\n");
        break;
      }
      if (strlen(pCorePath) != write(iFd2, pCorePath, strlen(pCorePath))) {
        DEMOPRT((char *)"set core_pattern fail!\n");
        close(iFd2);
        break;
      }
      close(iFd2);
    }

    /// 4, set core dump open succ
    iRet = 0;
    // DEMOPRT((char*)"set core dump open succ!\n");
  } while (0);

  return iRet;
}

/**
 * @brief            生成core dump需要的路径,参数设置
 *
 * @param[in]        none
 *
 * @param[out]       none
 *
 * @return           none
 */
void demo_set_core_dump(void) {
  char szCorePid[32];
  char szCorePath[128] = {"demo_core_dump"};

  /*为了唯一区分core文件,文件名格式追加IP地址及设备序列号*/
  strcpy(szCorePid, "1");

  /*core保存目录*/
  strcpy(szCorePath, "/coredump/");
  /// 判断g_stat_dsta是否有写权限,若无则返回错误
  int re = access(szCorePath, W_OK);
  if (re != 0) {
    re = mkdir(szCorePath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (0 != re) {
      perror("mkdir dir /coredump/  error!!!!!!\n");
    }
  }

  /*core 系统参数信息 */
  strcat(szCorePath, "core-%s-%e-%p-%t");

  if (0 != demo_core_dump_open(szCorePid, szCorePath)) {
    printf("<demo> ========= gdb core dump open fail! =========\n\n");
  } else {
    printf("<demo> sys gdb core open succ ~~~ \n");
  }
}

static void sigsegv_handler(int signum, siginfo_t *info, void *ptr) {
  static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};
  // DEMOPRT((char*)"######## info.si_signo = %d\n", signum);
  // DEMOPRT((char*)"######## info.si_errno = %d\n", info->si_errno);
  if ((info->si_code < 0) ||
      (info->si_code >= sizeof(si_codes) / sizeof(char *))) {
    DEMOPRT((char *)"######## info.si_code  = %d\n", info->si_code);
  } else {
    DEMOPRT((char *)"######## info.si_code  = %d (%s)\n", info->si_code,
            si_codes[info->si_code]);
  }
  DEMOPRT((char *)"######## info.si_addr  = %p\n", info->si_addr);

  if ((SIGSEGV == signum) || (SIGFPE == signum) || (SIGINT == signum) ||
      (SIGTERM == signum) || (SIGKILL == signum)) {
    ucontext_t *ucontext = (ucontext_t *)ptr;

    /*for arm*/
    DEMOPRT((char *)"######## the arm_fp 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_fp);
    DEMOPRT((char *)"######## the arm_ip 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_ip);
    DEMOPRT((char *)"######## the aarch64_sp 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_sp);
    DEMOPRT((char *)"######## the arm_lr 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_lr);
    DEMOPRT((char *)"######## the aarch64_pc 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_pc);
    DEMOPRT((char *)"######## the arm_cpsr 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.arm_cpsr);
    DEMOPRT((char *)"######## the falut_address 0x%3x\n",
            (unsigned int)ucontext->uc_mcontext.fault_address);
    DEMOPRT((char *)"######## Stack trace (non-dedicated):");
    /*int sz = backtrace(bt, 20);
    printf("the stack trace is %d\n",sz);
    strings = backtrace_symbols(bt, sz);
    for(i = 0; i < sz; ++i){
            printf("%s\n", strings[i]);
    }*/
    //_exit (-1);
  }
  DEMOPRT((char *)"\nrun demo_signal_clean\n");
  demo_signal_clean();
  signal(signum, SIG_DFL);
}

void catch_sigsegv(void) {
  struct sigaction action;

  // signal acquisition[must must must add]
  memset(&action, 0, sizeof(action));
  action.sa_sigaction = sigsegv_handler;
  action.sa_flags = SA_SIGINFO;
  // illegal access memory
  if (sigaction(SIGSEGV, &action, NULL) < 0) {
    perror("sigaction");
  }

  // interrupt
  if (sigaction(SIGINT, &action, NULL) < 0) {
    perror("sigaction");
  }

  // abort
  if (sigaction(SIGTERM, &action, NULL) < 0) {
    perror("sigaction");
  }

  // math processing
  if (sigaction(SIGFPE, &action, NULL) < 0) {
    perror("sigaction");
  }

  // memory crossover
  // SIGKILL 和SIGSTOP不支持信号自定义

  if (sigaction(SIGABRT, &action, NULL) < 0) {
    perror("sigaction");
  }

  if (sigaction(SIGBUS, &action, NULL) < 0) {
    perror("sigaction");
  }

  struct sigaction actionPipe;
  memset(&actionPipe, 0, sizeof(actionPipe));
  actionPipe.sa_handler = SIG_IGN;
  actionPipe.sa_flags = 0;

  /// 增加SIGPIPE信号忽略
  /// TODO:  [WARNING]此处报错,待查
  if (sigaction(SIGPIPE, &actionPipe, 0) < 0) {
    perror("sigaction");
  }
}

/**
 * @brief            main function
 *
 * @param[in] 		none
 *
 * @return           0 if successful, otherwise an error number returned
 */
int main(int argc, char *argv[]) {
  int i, ret = 0;
  // printf("main bf\n");
  // system("free");
  openlog("hik_demo", LOG_CONS, LOG_LOCAL2);

  ///< the situation:input param
  if (argc != 3) {
    DEMOPRT((char *)"please input configure file\n");
    DEMOPRT((char *)"eg: ./opdevsdkDemo_caffe yolo_caffe_config.txt YUV or "
                    "./opdevsdkDemo_caffe yolo_caffe_config.txt FILE\n");
    return -1;
  }

  if ((0 != strcmp("YUV", argv[2])) && (0 != strcmp("FILE", argv[2]))) {
    DEMOPRT((char *)"please input argv[2] 'FILE' or 'YUV'\n");
    return -1;
  }

  for (i = 0; i < argc; i++) {
    printf("argv[%d] %s\n", i, argv[i]);
  }

  pthread_t yuv_proc_tid;
  pthread_t yuv_proc_tid_2;
  pthread_t socket_proc_tid;
#ifdef TEST
  /// set core dump for demo
  // demo_set_core_dump();
#endif

  catch_sigsegv();

  // sleep(8);
  //!!!注意!!!,当前demo示例中会有大量打印，DEMOPRT直接调用函数devsdk_print，会在设备目录(/home/opdevsdk/.../dlog/，可能有调整)下记录log信息，
  // 实际项目中，不建议将所有打印信息记录，建议精简记录，仅记录有用信息，
  DEMOPRT((char *)"########################################### opdevsdk debug  "
                  "    #########################################\n");

  sleep(1);

  DEMOPRT((char *)"--- demo_init_test start\n");
  // demo system init
  demo_init_test();
  DEMOPRT((char *)"--- demo_init_test end\n");
  // printf("2 bf\n");
  // system("free");

  demo_init_socket_server();
  DEMOPRT((char *)"--- demo_init_socket_server end\n");

  DEMOPRT((char *)"--- demo_scheduler_test start\n");
  // scheduler init

  demo_scheduler_test();
  DEMOPRT((char *)"--- demo_scheduler_test end\n");

  DEMOPRT((char *)"--- demo_alg_init start\n");
  // hikflow init
  demo_alg_init(argv[1]);
  DEMOPRT((char *)"--- demo_alg_init end\n");
  thread_flg = 1;

  if (0 == strcmp("FILE", argv[2])) {
    /*algorithm processing thread*/
    pthread_create(&yuv_proc_tid, NULL, demo_alg_proc_from_file_thread, NULL);
    pthread_join(yuv_proc_tid, NULL);
  } 
  else if (0 == strcmp("YUV", argv[2])) {
    // TODO:  视频流
    demo_alg_get_res(&alg_width, &alg_height);
    DEMOPRT((char*)"--- alg_width %d alg_height %d MSCALE_TEST %d\n", alg_width, alg_height, MSCALE_TEST);

    int grpId = 0;
    void *ms_hdl = NULL;
    int chan = 0;
    OPDEVSDK_VIN_CHN_ST vin_info = {0};
    // TODO:  理解一下
#if MSCALE_TEST
    ret = demo_mscale_prep(&chan, &grpId, &ms_hdl, &vin_info);
    if (ret != 0) {
      DEMOPRT((char *)"demo_mscale_prep error ret = 0x%x\n", ret);
      return ret;
    }
    groupId = grpId;
    mscale_hdl = ms_hdl;
#else
    // 开启vin
    OPDEVSDK_SYS_ABILITY_ST abili = {0};
    opdevsdk_sys_getAbility(&abili);
    if (abili.vinAbili.chnNum < 1) {
      DEMOPRT((char *)"vinAbili.chnNum %d err\n", abili.vinAbili.chnNum);
      return -1;
    }

    chan = abili.vinAbili.chnInfo[0].chan;

    ret = opdevsdk_vin_init(chan);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_init error ret = 0x%x\n", ret);
      return -1;
    }
    bvin_init = OP_TRUE;
    // DEMOPRT((char*)"opdevsdk_vin_init chan %d\n", chan);

    // 设置vi帧率
    ret = opdevsdk_vin_setFrameRate(chan, OPDEVSDK_VIDEO_FRAME_RATE_6_25);
    if (ret != OPDEVSDK_S_OK) {
      DEMOPRT((char *)"opdevsdk_vin_setFrameRate error ret = 0x%x\n", ret);
      return -1;
    }

    OPDEVSDK_VIN_CHN_ST *pchn_info = &abili.vinAbili.chnInfo[0];
    memcpy(&vin_info, pchn_info, sizeof(OPDEVSDK_VIN_CHN_ST));
#endif
    input_width = vin_info.width;
    input_height = vin_info.height;
    DEMOPRT((char*)"chan %d input w %d h %d\n", chan, input_width,
    input_height);

    thread_flg = 1;
#if MSCALE_TEST
    {
      /*getting YUV frames to algorithm buffer*/
      ret =
          pthread_create(&yuv_proc_tid, NULL, demo_vin_get_thread, mscale_hdl);
      // 线程优先级默认填80，栈大小可填0(当前内部限制最低2M),args为透传参数,不需要填0，后面不填透传参数
      if (ret != 0) {
        DEMOPRT((char *)"devsdk_pthrd_creat_pthread error ret = 0x%x\n", ret);
        return ret;
      }
      // pthread_create(&yuv_proc_tid, NULL, (void *)demo_vin_get_thread, NULL);

      /*algorithm processing*/
      ret = pthread_create(&yuv_proc_tid_2, NULL, demo_alg_proc_thread_2, NULL);
      if (ret != 0) {
        DEMOPRT((char *)"devsdk_pthrd_creat_pthread error ret = 0x%x\n", ret);
        return ret;
      }
      // pthread_create(&yuv_proc_tid_2, NULL, (void *)demo_alg_proc_thread_2,
      // NULL);

      /*socket connection processing*/
      ret = pthread_create(&socket_proc_tid, NULL, demo_socket_proc, NULL);
      if (ret != 0) {
        DEMOPRT((char *)"socket_pthrd_creat_pthread error ret = 0x%x\n", ret);
        return ret;
      }
    }
#else
    {
      /*Here's an example: getting YUV frames from media driver and adding
       * intelligent targets to display in HK WEB*/
      // demo_yuv_buf_init(&yuv_buf,input_width,input_height);
      demo_alg_buf_init(&alg_buf, alg_width, alg_height);

      /*getting YUV frames to algorithm buffer*/
      ret = pthread_create(&yuv_proc_tid, NULL, &demo_video_get_thread,
                           NULL);
      // 线程优先级默认填80，栈大小可填0(当前内部限制最低2M),args为透传参数,不需要填0，后面不填透传参数
      if (ret != 0) {
        DEMOPRT((char *)"devsdk_pthrd_creat_pthread error ret = 0x%x\n", ret);
        return ret;
      }

      /*algorithm processing*/
      ret = pthread_create(&yuv_proc_tid_2, NULL, &demo_alg_proc_thread,
                           NULL);
      if (ret != 0) {
        DEMOPRT((char *)"devsdk_pthrd_creat_pthread error ret = 0x%x\n", ret);
        return ret;
      }
    }
#endif

    while (1) {
      usleep(20 * 1000);
    }
  }
}
