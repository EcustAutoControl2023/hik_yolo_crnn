/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_isp.h
* @brief        基础服务库ISP模块
*
* @date         2020/3/31
* @version      2.0.0
* @note         增加luma/expose相关接口.
*****************************************************************************/

#ifndef _OPDEVSDK_ISP_H_
#define _OPDEVSDK_ISP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"

#define OPDEVSDK_ISP_MAX_TARGET_NUM         (128)       /*!< ISP模块支持的最大目标数量 */

/**
* @brief 曝光信息
*/
typedef struct _OPDEVSDK_ISP_EXPOSE_INFO_ST_
{
    int                 enable;         /*!< 曝光使能,OP_FALSE-关闭,OP_TRUE-开启 */
    unsigned int        refBright;      /*!< 曝光亮度等级,范围[0,100] */
    unsigned int        time;           /*!< 曝光持续时间,范围[1,60],单位:分钟 */
} OPDEVSDK_ISP_EXPOSE_INFO_ST;


/**
* @brief 目标亮度信息
*/
typedef struct _OPDEVSDK_ISP_LUMA_INFO_ST_
{
    unsigned int        tgtNum;         /*!< 目标数量 */
    unsigned int        *lumaValue;     /*!< 所有目标平均亮度,指针类型,亮度范围[0,255] */
    unsigned int        *score;         /*!< 所有目标平均评分，指针类型,评分范围[0,100] */
} OPDEVSDK_ISP_LUMA_INFO_ST;


/** 
* @brief            设置当前通道目标亮度信息
*
* @param[in]        chan            前端通道号
* @param[in]        pLuma           目标亮度信息，指针类型
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_isp_setLuma(int chan,OPDEVSDK_ISP_LUMA_INFO_ST *pLuma);


/** 
* @brief            设置曝光控制参数
*
* @param[in]        chan            前端通道号
* @param[in]        pExpose         人脸曝光信息，指针类型
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_isp_setExpo(int chan,OPDEVSDK_ISP_EXPOSE_INFO_ST *pExpose);

/** 
* @brief           isp增益等级获取
*
* @param[in]        chan            通道id
* @param[out]       level           isp增益等级
*                   
* @return           0 成功  非0失败。
*/
OPDEVSDK_EXPORT int opdevsdk_isp_getGainLevel(int chan, int *level);

/** 
* @brief           isp透雾等级获取
*
* @param[in]        chan            通道id
* @param[out]       level           isp透雾等级
*                   
* @return           0 成功  非0失败。
*/
OPDEVSDK_EXPORT int opdevsdk_isp_getFogLevel(int chan, int *level);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_ISP_H_ */


