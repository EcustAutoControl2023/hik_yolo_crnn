/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_video.h
* @brief        基础服务库视频类头文件
*
* @date         2020/8/13
* @version      2.0.0
* @note         分离视频相关结构体
*****************************************************************************/

#ifndef _OPDEVSDK_VIDEO_H_
#define _OPDEVSDK_VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"


/**
* @brief YUV图像存储格式
*/
typedef enum _OPDEVSDK_VIDEO_PIXEL_FORMAT_EN_
{
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YUV422     = 0,         /*!< YUV4:2:2,YUV交替存储,例如:YUYV */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_NV12       = 1,         /*!< YUV4:2:0,Y平面存储,UV交替存储,且U在前 */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_NV21       = 2,         /*!< YUV4:2:0,Y平面存储,UV交替存储,且V在前,默认格式 */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YUV400     = 3,         /*!< YUV4:0:0,仅Y分量 */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_I420       = 4,         /*!< YUV4:2:0,I420也叫YU12,Y,U,V按次序依次平面储存 */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YV12       = 5,         /*!< YUV4:2:0,Y,V,U按次序依次平面储存 */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_MAX_NUM    = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_PIXEL_FORMAT_EN;


/**
* @brief  图像数据格式
*/
typedef enum _OPDEVSDK_VIDEO_DATA_TYPE_EN_
{
    OPDEVSDK_VIDEO_DATA_TYPE_VI_FRAME             = 0,      /*!< 输入视频帧 */
    OPDEVSDK_VIDEO_DATA_TYPE_CAPTURE_IMAGE        = 1,      /*!< 输入抓拍帧 */
    OPDEVSDK_VIDEO_DATA_TYPE_CUSTOM               = 2,      /*!< 定制帧 */
    OPDEVSDK_VIDEO_DATA_TYPE_MAX_NUM              = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_DATA_TYPE_EN;


/**
* @brief  视频帧率信息
*/
typedef enum _OPDEVSDK_VIDEO_FRAME_RATE_EN_
{
    OPDEVSDK_VIDEO_FRAME_RATE_FULL      = 0,        /*!< 视频全帧率(最大帧率) */
    OPDEVSDK_VIDEO_FRAME_RATE_1         = 1,        /*!< 视频帧率:1 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_6_25      = 2,        /*!< 视频帧率:6.25 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_8         = 3,        /*!< 视频帧率:8 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_10        = 4,        /*!< 视频帧率:10 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_12_5      = 5,        /*!< 视频帧率:12.5 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_15        = 6,        /*!< 视频帧率:15 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_20        = 7,        /*!< 视频帧率:20 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_25        = 8,        /*!< 视频帧率:25 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_30        = 9,        /*!< 视频帧率:30 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_MAX_NUM   = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_FRAME_RATE_EN;

/**
* @brief  图像压缩模式
*/
typedef enum _OPDEVSDK_COMPRESS_MODE_EN_
{
    OPDEVSDK_COMPRESS_MODE_NONE         = 0,        /*!< 无压缩 */
    OPDEVSDK_COMPRESS_MODE_MAX_NUM      = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
} OPDEVSDK_COMPRESS_MODE_EN;

/**
* @brief 一帧图像描述
*/
typedef struct _OPDEVSDK_VIDEO_FRAME_ST_
{
    OPDEVSDK_VIDEO_PIXEL_FORMAT_EN      dataFormat;         /*!< 图像存储格式,定义参考OPDEVSDK_VIN_DATA_FORMAT_EN */
    int                                 width;              /*!< 图像宽度,大小16像素对齐 */
    int                                 height;             /*!< 图像高度,大小8像素对齐 */
    int                                 pitch[3];           /*!< 图像分量跨距 */
    void                                *pVirAddr[3];       /*!< 图像分量虚拟地址 */
    void                                *pPhyAddr[3];       /*!< 图像分量物理地址 */

    OPDEVSDK_COMPRESS_MODE_EN 			enCompressMode;		/*!< 图像压缩方式 */
    int                              	res[5];        	/*!< 预留字段 */

} OPDEVSDK_VIDEO_FRAME_ST;


/**
* @brief 一帧图像完整描述(包括携带数据)
*/
typedef struct _OPDEVSDK_VIDEO_FRAME_INFO_ST_
{
    unsigned int                        chan;           /*!< 通道号 */
    unsigned long long                  timeStamp;      /*!< 时间戳,单位:微秒 */
    unsigned int                        frmNum;         /*!< 图像帧号 */
    OPDEVSDK_VIDEO_DATA_TYPE_EN         dataType;       /*!< 图像数据格式,定义参考OPDEVSDK_VIDEO_DATA_TYPE_EN */
    unsigned int                        dataLen;        /*!< 私有数据长度,仅当dataType=OPDEVSDK_VIDEO_DATA_TYPE_CUSTOM模式下有效 */

    OPDEVSDK_MEDIADRV_TIME_ST           dateTime;       /*!< 图像帧全局时间 */
    OPDEVSDK_VIDEO_FRAME_ST             yuvFrame;       /*!< 一帧图像描述 */
    int                                 res[6];        /*!< 预留字段 */
} OPDEVSDK_VIDEO_FRAME_INFO_ST;


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_VIDEO_H_ */


