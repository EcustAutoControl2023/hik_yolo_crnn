/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mscale.h
* @brief        基础库multi-scale(一进多出缩放)模块
*
* @date         2020/3/31
* @version      2.0.0
* @note         增加描述信息和mscale相关接口
*               
*****************************************************************************/

#ifndef _OPDEVSDK_MSCALE_H_
#define _OPDEVSDK_MSCALE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_vin.h"


#define OPDEVSDK_MSCALE_MAX_OUT_RES_NUM         (8)     /*!< multi-scale支持的单通道最大输出缩放分辨率数量 */ 

/**
* @brief  图像分辨率信息
*/
typedef struct _OPDEVSDK_MSCALE_RES_ST_
{
    int     width;          /*!< 图像宽度,单位:像素,8像素对齐 */
    int     pitch;          /*!< 图像跨距,单位:像素,16像素对齐 */
    int     height;         /*!< 图像高度,单位:像素,8像素对齐 */
} OPDEVSDK_MSCALE_RES_ST;


/**
* @brief  multi-scale初始化描述，包含一进多出图像信息
*/
typedef struct _OPDEVSDK_MSCALE_INIT_ST_
{
    OPDEVSDK_MSCALE_RES_ST      srcImg;                                         /*!< 输入源图信息 */
    int                         dstImgCnt;                                      /*!< 输出缩放目的图像分辨率数量 */
    OPDEVSDK_MSCALE_RES_ST      dstImg[OPDEVSDK_MSCALE_MAX_OUT_RES_NUM];        /*!< 输出缩放目的图像信息 */
} OPDEVSDK_MSCALE_INIT_ST;


/** 
* @brief            创建一路缩放通道(multi-scale)句柄
*
* @param[out]       handle          返回缩放通道句柄，指针类型
* @param[in]        pInput          初始化参数
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_create(void** handle, OPDEVSDK_MSCALE_INIT_ST *pInput);


/** 
* @brief            将VB缓存池组与缩放通道句柄绑定
* @note             grpId由opdevsdk_mem_createVbPoolGrp创建，是multi-scale运行的内存基础
*
* @param[in]        handle          缩放通道句柄
* @param[in]        grpId           VB缓存池组ID
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_bindVbPoolGrp(void *handle,int grpId);


/** 
* @brief            将VB缓存池组与缩放通道句柄解绑
*
* @param[in]        handle          缩放通道句柄
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_unbindVbPoolGrp(void *handle);


/** 
* @brief            销毁缩放通道(multi-scale)句柄
*
* @param[in]        handle          缩放通道句柄
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_destroy(void *handle);


/** 
* @brief            开启缩放通道
*
* @param[in]        handle          缩放通道句柄
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_start(void *handle);


/** 
* @brief            关闭缩放通道
*
* @param[in]        handle          缩放通道句柄
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_stop(void *handle);

/** 
* @brief            发送一帧VBC图像数据到缩放通道
*
* @param[in]        handle          缩放通道句柄
* @param[in]        vbc             vbc数据，指针类型
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_sendVbc(void *handle,void *vbc);


/** 
* @brief            发送一帧图像数据到缩放通道
*
* @param[in]        handle          缩放通道句柄
* @param[in]        pFrame          图像数据描述，指针类型
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_sendFrame(void *handle,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            从缩放通道获取一帧指定分辨率的图像数据
*
* @param[in]        handle          缩放通道句柄
* @param[in]        img             图像分辨率信息
* @param[out]       pFrame          图像数据描述，指针类型
* @param[in]        timeout         超时时间,单位:ms,<-1为非法参数,-1为阻塞获取,0为非阻塞获取,其它为最大超时时间获取
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_getFrame(void *handle,OPDEVSDK_MSCALE_RES_ST img,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame,int timeout);


/** 
* @brief            释放一帧图像数据
*
* @param[in]        handle          缩放通道句柄
* @param[in]        pFrame          图像数据描述，指针类型
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_releaseFrame(void *handle,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            从缩放通道获取一帧指定分辨率的VBC信息
* @note             VBC信息存储的也是图像信息
*
* @param[in]        handle          缩放通道句柄
* @param[in]        img             待申请的图像分辨率描述
* @param[out]       pVbc            VBC数据指针
* @param[in]        timeout         超时时间,单位:毫秒,<-1为非法参数,-1为阻塞获取,0为非阻塞获取,其它为最大超时时间获取
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_getVbc(void *handle,OPDEVSDK_MSCALE_RES_ST img,void **pVbc,int timeout);


/** 
* @brief            释放一帧VBC图像数据
*
* @param[in]        pVbc            VBC数据指针
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_releaseVbc(void *vbc);


#ifdef __cplusplus
}
#endif


#endif //_OPDEVSDK_MSCALE_H_

