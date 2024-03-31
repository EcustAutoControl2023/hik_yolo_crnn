/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_jpegdec.h
* @brief        基础服务库JPEG解码模块
*
* @date         2021/2/18
* @version      2.0.0
* @note         增加JPEG解码处理接口
*****************************************************************************/


#ifndef _OPDEVSDK_JPEGDEC_H_
#define _OPDEVSDK_JPEGDEC_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_img.h"

#define OPDEVSDK_JPEGDEC_MAX_HANDLE         (4)             /*!< JPEG解码模块支持的最大通道数量 */

#define OPDEVSDK_JPEGDEC_MAX_WIDTH          (2560)          /*!< JPEG解码模块支持的最大宽度,单位:像素 */
#define OPDEVSDK_JPEGDEC_MAX_HEIGHT         (1440)          /*!< JPEG解码模块支持的最大高度,单位:像素 */
#define OPDEVSDK_JPEGDEC_MIN_WIDTH          (8)             /*!< JPEG解码模块支持的最小宽度,单位:像素 */
#define OPDEVSDK_JPEGDEC_MIN_HEIGHT         (8)             /*!< JPEG解码模块支持的最小高度,单位:像素 */


/**
* @brief JPEG解码能力,按位使能,例如:0x3表示支持硬件解码和软件解码
*/
typedef enum _OPDEVSDK_JPEGDEC_ABILI_EN_
{
    OPDEVSDK_JPEGDEC_UNSUPPORT      = 0,        /*!< 不支持,无法使用JPEG解码模块相关接口 */
    OPDEVSDK_JPEGDEC_HARDWARE       = 1,        /*!< 支持JPEG硬件解码 */
    OPDEVSDK_JPEGDEC_SOFT           = 2,        /*!< 支持JPEG软件件解码 */
    OPDEVSDK_JPEGDEC_MAX_NUM        = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_JPEGDEC_ABILI_EN;


/**
* @brief JPEG数据参数
*/
typedef struct _OPDEVSDK_JPEGDEC_PARAM_ST_
{
    void                    *pBuf;          /*!< JPEG图像输入缓存地址 */
    unsigned int            jpegSize;       /*!< JPEG图像数据大小，单位Byte */
    int                     res[12];        /*!< 保留字段 */
} OPDEVSDK_JPEGDEC_PARAM_ST;


/** 
* @brief            创建一路JPEG解码通道
*
* @param[out]       handle          JPEG解码句柄,指针类型
* @param[in]        bSoftEnc        软件编码标识,FALSE表示使用硬件解码;TRUE表示使用软件解码
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_create(void **handle,int bSoftDec);


/** 
* @brief            JPEG解码处理
*
* @param[in]        handle          JPEG解码句柄
* @param[in]        pParam          JPEG数据描述
* @param[out]       pFrame          解码后YUV数据描述，指针类型
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_proc(void *handle,OPDEVSDK_JPEGDEC_PARAM_ST *pParam,OPDEVSDK_VIDEO_FRAME_ST *pFrame);


/** 
* @brief            销毁JPEG解码通道
*
* @param[in]        handle          JPEG解码句柄
* @param[out]       none                        
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_destroy(void *handle);


#ifdef __cplusplus
}
#endif

#endif //_OP_DEVSDK_JPEGDEC_H_

