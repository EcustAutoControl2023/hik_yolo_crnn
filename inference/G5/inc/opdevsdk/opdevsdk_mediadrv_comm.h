/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mediadrv_comm.h
* @brief        �����⹫��ͷ�ļ�
*
* @date         2019/2/18
* @version      1.0.0
* @note         �����ڴ�ӿ�
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ�
*
* @date         2020/2/19
* @version      2.0.1
* @note         ����OP_TRUE,OP_FALSE��غ궨��
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
* @brief ����ֵ
*/
#define OPDEVSDK_S_OK       (0)     /*!< ��������ⷵ��ֵ��0����ɹ� ������ֵ��ο������ĵ� */

/**
* @brief ��������
*/
#define OPDEVSDK_MEDIADRV_ENUM_MAXI             (0xffffffff)        /*!< �޷�������(unsigned int)�����ֵ */
#define OPDEVSDK_MEDIADRV_ALIGN                 (128)               /*!< ����ߴ� */
#define OPDEVSDK_MEDIADRV_MAX_CHAR_NUM          (64)                /*!< ֧���ַ�������ֽڳ��� */

/**
* @brief color���㹫ʽ
*/
#define OPDEVSDK_MEDIADRV_GET_COLOR_Y(x)        ((x)&0xff)
#define OPDEVSDK_MEDIADRV_GET_COLOR_U(x)        (((x)>> 8)&0xff)
#define OPDEVSDK_MEDIADRV_GET_COLOR_V(x)        (((x)>>16)&0xff)
#define OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)     ((y&0xff)|((u&0xff)<<8)|((v&0xff)<<16))


/**
* @brief OP_TRUE,OP_FALSE����
*/
#ifndef OP_TRUE
#define OP_TRUE        1
#endif

#ifndef OP_FALSE
#define OP_FALSE       0
#endif


/**
* @brief ȫ��ʱ����� 
*/
typedef struct _OPDEVSDK_MEDIADRV_TIME_ST_
{
    short       year;           /*!< �� */
    short       month;          /*!< �� */
    short       dayOfWeek;      /*!< ���ڼ�,0��Ӧ������ */
    short       day;            /*!< �� */
    short       hour;           /*!< ʱ */
    short       minute;         /*!< �� */
    short       second;         /*!< �� */
    short       milliSecond;    /*!< ���� */
}OPDEVSDK_MEDIADRV_TIME_ST;


#ifdef __cplusplus
}
#endif


#endif

