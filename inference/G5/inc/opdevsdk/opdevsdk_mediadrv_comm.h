/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mediadrv_comm.h
* @brief        基础库公共头文件
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加内存接口
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口
*
* @date         2020/2/19
* @version      2.0.1
* @note         增加OP_TRUE,OP_FALSE相关宏定义
*****************************************************************************/


#ifndef _OPDEVSDK_MEDIADRV_COMM_H_
#define _OPDEVSDK_MEDIADRV_COMM_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifndef OPDEVSDK_EXPORT
#define OPDEVSDK_EXPORT __attribute__((visibility("default")))
#endif

/**
* @brief 返回值
*/
#define OPDEVSDK_S_OK       (0)     /*!< 基础服务库返回值，0代表成功 ，其他值请参考开发文档 */

/**
* @brief 公共定义
*/
#define OPDEVSDK_MEDIADRV_ENUM_MAXI             (0xffffffff)        /*!< 无符号整形(unsigned int)最大数值 */
#define OPDEVSDK_MEDIADRV_ALIGN                 (128)               /*!< 对齐尺寸 */
#define OPDEVSDK_MEDIADRV_MAX_CHAR_NUM          (64)                /*!< 支持字符串最大字节长度 */

/**
* @brief color计算公式
*/
#define OPDEVSDK_MEDIADRV_GET_COLOR_Y(x)        ((x)&0xff)
#define OPDEVSDK_MEDIADRV_GET_COLOR_U(x)        (((x)>> 8)&0xff)
#define OPDEVSDK_MEDIADRV_GET_COLOR_V(x)        (((x)>>16)&0xff)
#define OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)     ((y&0xff)|((u&0xff)<<8)|((v&0xff)<<16))


/**
* @brief OP_TRUE,OP_FALSE定义
*/
#ifndef OP_TRUE
#define OP_TRUE        1
#endif

#ifndef OP_FALSE
#define OP_FALSE       0
#endif


/**
* @brief 全局时间参数 
*/
typedef struct _OPDEVSDK_MEDIADRV_TIME_ST_
{
    short       year;           /*!< 年 */
    short       month;          /*!< 月 */
    short       dayOfWeek;      /*!< 星期几,0对应星期天 */
    short       day;            /*!< 日 */
    short       hour;           /*!< 时 */
    short       minute;         /*!< 分 */
    short       second;         /*!< 秒 */
    short       milliSecond;    /*!< 毫秒 */
}OPDEVSDK_MEDIADRV_TIME_ST;


#ifdef __cplusplus
}
#endif


#endif

