/***************************************************************************
* note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         demo_comm.h
* @brief        the interface of media Demo
*
* @author       guanjiajun
*
* @date         2019/2/18
* @version      1.0.0
* @note         add demo for mediaDrv and hikflow
*
* @date         2019/6/26
* @version      2.0.0
* @note         Synchronized mediaDrv Interface
*****************************************************************************/


#ifndef _DEMO_COMM_H_
#define _DEMO_COMM_H_

#include "opdevsdk_video.h"
#include "opdevsdk_mem.h"
#ifdef __cplusplus
extern "C" {
#endif

void demo_syslog_print(char *format, ...);
/**
* @brief demo common function
*/
#define DEMOPRT 									 demo_syslog_print
#define DEMO_CHECK_RET(c, r)                         if(c){DEMOPRT((char*)"DEMO_ASSERT_RET FAILED!\n"); return r;}
#define DEMO_CHECK_RET_EX(c)                         if(c){DEMOPRT((char*)"DEMO_ASSERT_RET FAILED!\n"); return;}


/**
* @brief demo buffer number
*/
#define DEMO_MAX_BUF_COUNT                            (3)
#define DEMO_MAX_ALG_BUF_COUNT                        (2)


/**
* @brief choose 64 bits or 32 bits
*/
#if defined(ARM_64)
typedef unsigned long long                            PTR_VOID;
#else
typedef unsigned int                                PTR_VOID;
#endif


/**
* @brief YUV buffer struct
*/
typedef struct _DEMO_YUV_BUF_
{
    int                                             buf_w;
    int                                             buf_r;
    int                                             buf_size;
    OPDEVSDK_VIDEO_FRAME_INFO_ST                    frm[DEMO_MAX_BUF_COUNT];
}DEMO_YUV_BUF;


/**
* @brief algorithm buffer struct
*/
typedef struct _DEMO_ALG_BUF_
{
    int                                             buf_w;
    int                                             buf_r;
    int                                             buf_size;
    OPDEVSDK_VIDEO_FRAME_INFO_ST                    frm[DEMO_MAX_ALG_BUF_COUNT];
}DEMO_ALG_BUF;


#ifdef __cplusplus
}
#endif

#endif

