/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_vin.h
* @brief        基础库视频输入模块接口
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加new/put frame相关接口
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口,删除状态统计
*
* @date         2020/2/19
* @version      2.0.1
* @note         增加接口描述和VBC相关接口
*****************************************************************************/

#ifndef _OPDEVSDK_VIN_H_
#define _OPDEVSDK_VIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_video.h"

#define OPDEVSDK_VIN_CHN_MAX_NUM        (16)        /*!< 基础服务库支持最大输入通道数量 */


/**
* @brief 通道描述
*/
typedef struct _OPDEVSDK_VIN_CHN_ST_
{
    int             chan;           /*!< 输入通道号 */
    int   			fovChn;       	/*!< fov通道号,从0开始,支持扩展通道 */    
    int             width;          /*!< 输入通道最大宽度,单位:像素 */
    int             height;         /*!< 输入通道最大高度,单位:像素 */
    float           fps;            /*!< 输入通道最大帧率,浮点型,单位:帧/秒 */
    int             res[4];         /*!< 预留字段 */
}OPDEVSDK_VIN_CHN_ST;

/**
* @brief 设备输入通道描述
*/
typedef struct _OPDEVSDK_VIN_ABILITY_ST_
{
    int                      chnNum;                                /*!< 输入通道数量 */
    OPDEVSDK_VIN_CHN_ST      chnInfo[OPDEVSDK_VIN_CHN_MAX_NUM];     /*!< 每个输入通道描述 */
}OPDEVSDK_VIN_ABILITY_ST;

/** 
* @brief            视频输入模块初始化 
*
* @param[in]        chan            前端通道号
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_init(int chan);


/** 
* @brief            视频输入模块去初始化
*
* @param[in]        none
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_deinit(int chan);


/** 
* @brief            设置获取视频的帧率
* @note             需要指定通道，默认1
*
* @param[in]        chan            前端通道号
* @param[in]        frameRate       设定的视频帧率 
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_setFrameRate(int chan,OPDEVSDK_VIDEO_FRAME_RATE_EN frameRate);


/** 
* @brief            获取一帧前端图像数据
* @note             需要指定通道，默认1
*
* @param[in]        chan            前端通道号
* @param[in/out]    pFrame          图像描述，指针类型，需要指定分辨率
* @param[in]        timeout         超时时间,单位:毫秒,timeout<-1为非法参数,-1为阻塞获取,0为非阻塞获取,其它为最大超时时间获取
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_getFrame(int chan,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame,int timeout);


/** 
* @brief            释放一帧图像数据
* @note             需要指定通道，默认1,与getFrame成对使用
*
* @param[in]        chan            前端通道号
* @param[in/out]    pFrame          图像描述，指针类型，需要指定分辨率
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_releaseFrame(int chan,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            获取一帧VBC图像数据
*
* @param[in]        chan            前端通道号
* @param[in]        timeout         超时时间,单位:毫秒,timeout<-1为非法参数,-1为阻塞获取,0为非阻塞获取,其它为最大超时时间获取
* @param[out]       pVbc            指向VBC类型指针的指针
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_getVbc(int chan,void **pVbc,int timeout);


/** 
* @brief            释放一帧VBC图像数据
* @note             需要指定通道，默认1,与getVbc成对使用
*
* @param[in]        chan            前端通道号
* @param[in]        vbc             指向VBC类型的指针
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_vin_releaseVbc(int chan,void *vbc);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_VIN_H_ */


