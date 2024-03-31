/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_sys.h
* @brief        基础库系统模块
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加sys能力
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步内部接口
*
* @date         2020/2/25
* @version      2.0.1
* @note         增加jpeg解码能力
*****************************************************************************/


#ifndef _OPDEVSDK_SYS_H_
#define _OPDEVSDK_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_jpegenc.h"
#include "opdevsdk_jpegdec.h"
#include "opdevsdk_vin.h"


/**
* @brief 数据透传支持的最大link个数
*/
#define   OPDEVSDK_SYS_TRANS_DATA_MAX_LINK      (2)

/**
* @brief 设备基础库能力
*/
typedef struct _OPDEVSDK_SYS_ABILITY_ST_
{
    int                             mmSize;             /*!< 智能可用MMZ内存,单位:KB,如果设置为0,表示不限制 */
    int                             cacheSize;          /*!< 智能可用cache MMZ内存,单位:KB,如果设置为0,表示不限制 */
    int                             noCacheSize;        /*!< 智能可用no cache MMZ内存,单位:KB,如果设置为0,表示不限制 */
    OPDEVSDK_JPEGENC_ABILI_EN       jpegEncAbili;       /*!< 设备jpeg编码能力,定义参考OPDEVSDK_JPEGENC_ABILI_EN */
    OPDEVSDK_JPEGDEC_ABILI_EN       jpegDecAbili;       /*!< 设备jpeg解码能力,定义参考OPDEVSDK_JPEGDEC_ABILI_EN */
    OPDEVSDK_IMG_SCALE_ABILI_EN     scaleAbili;         /*!< 设备图像缩放能力,定义参考OPDEVSDK_IMG_SCALE_ABILI_EN */
    OPDEVSDK_VIN_ABILITY_ST         vinAbili;           /*!< 设备输入通道能力,定义参考OPDEVSDK_SYS_VIN_ABILITY_ST */
    int                             res[12];            /*!< 预留字段 */
}OPDEVSDK_SYS_ABILITY_ST;


/**
* @brief 基础库版本信息
*/
typedef struct _OPDEVSDK_SYS_VERSION_ST_
{
    char        version[OPDEVSDK_MEDIADRV_MAX_CHAR_NUM];     /*!< 版本信息字符串 */
}OPDEVSDK_SYS_VERSION_ST;


/**
* @brief 透传数据信息,主要用于用户业务与多媒体进行某些数据通信,由双方约定好协议即可。
*/
typedef struct _OPDEVSDK_SYS_COM_DATA_ST_
{
    int                     chan;                   /*!< 前端通道号 */               
    unsigned long long      timeStamp;              /*!< 时间戳,单位毫秒 */
    unsigned short          version;                /*!< 版本信息,由外部通信定义 */
    const void              *send;                  /*!< 发送缓存 */
    int                     sendSize;               /*!< 发送数据大小 */
    void                    *recv;                  /*!< 接收数据缓存,可填NULL */
    int                     recvMaxSize;            /*!< 接收数据缓存上限,与recv配合使用 */
    int                     *recvSize;              /*!< 实际接收数据大小,与recv配合使用 */
    int                     timeout;                /*!< 超时时间,-1:阻塞,0:非阻塞,大于0:超时时间,单位毫秒 */
}OPDEVSDK_SYS_COM_DATA_ST;


/** 
* @brief            基础服务库初始化
*
* @param[in]        none
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_init(void);


/** 
* @brief            基础服务库去初始化
*
* @param[in]        none
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_deinit(void);


/** 
* @brief            获取基础服务库能力
*
* @param[in]        none
* @param[out]       pAbili          指向设备基础库能力的指针
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getAbility(OPDEVSDK_SYS_ABILITY_ST *pAbili);


/** 
* @brief            获取基础服务库版本信息
*
* @param[in]        none
* @param[out]       pVersion        基础服务库版本
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getVersion(OPDEVSDK_SYS_VERSION_ST *pVersion);


/** 
* @brief            获取系统时间
*
* @param[in]        none
* @param[out]       pTime           系统时间
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getGlobalTime(OPDEVSDK_MEDIADRV_TIME_ST *pTime);


/** 
* @brief            获取毫秒时间戳
*
* @param[in]        none
* @param[out]       none
* 
* @return           返回毫秒时间戳
*/
OPDEVSDK_EXPORT unsigned long long opdevsdk_sys_getMs(void);


/** 
* @brief            获取微秒时间戳
*
* @param[in]        none
* @param[out]       none
* 
* @return           返回微秒时间戳
*/
OPDEVSDK_EXPORT unsigned long long opdevsdk_sys_getUs(void);


/** 
* @brief            创建数据透传链路通道
*
* @param[in]        none
* @param[out]       handle         透传链路句柄，指针类型
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_createTransLink(void **handle);


/** 
* @brief            销毁数据透传链路
*
* @param[in]        handle         透传链路句柄
* @param[out]       none
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_destroyTransLink(void *handle);


/** 
* @brief            发送数据信息
*
* @param[in]        handle         透传链路句柄
* @param[in/out]    pData          传入的数据描述
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_sys_transLink(void *handle, OPDEVSDK_SYS_COM_DATA_ST *pData);


#ifdef __cplusplus
}
#endif

#endif //_OPDEVSDK_SYS_H


