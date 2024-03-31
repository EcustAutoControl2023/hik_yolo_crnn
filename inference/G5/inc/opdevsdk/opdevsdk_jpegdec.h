/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_jpegdec.h
* @brief        ���������JPEG����ģ��
*
* @date         2021/2/18
* @version      2.0.0
* @note         ����JPEG���봦��ӿ�
*****************************************************************************/


#ifndef _OPDEVSDK_JPEGDEC_H_
#define _OPDEVSDK_JPEGDEC_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_img.h"

#define OPDEVSDK_JPEGDEC_MAX_HANDLE         (4)             /*!< JPEG����ģ��֧�ֵ����ͨ������ */

#define OPDEVSDK_JPEGDEC_MAX_WIDTH          (2560)          /*!< JPEG����ģ��֧�ֵ������,��λ:���� */
#define OPDEVSDK_JPEGDEC_MAX_HEIGHT         (1440)          /*!< JPEG����ģ��֧�ֵ����߶�,��λ:���� */
#define OPDEVSDK_JPEGDEC_MIN_WIDTH          (8)             /*!< JPEG����ģ��֧�ֵ���С���,��λ:���� */
#define OPDEVSDK_JPEGDEC_MIN_HEIGHT         (8)             /*!< JPEG����ģ��֧�ֵ���С�߶�,��λ:���� */


/**
* @brief JPEG��������,��λʹ��,����:0x3��ʾ֧��Ӳ��������������
*/
typedef enum _OPDEVSDK_JPEGDEC_ABILI_EN_
{
    OPDEVSDK_JPEGDEC_UNSUPPORT      = 0,        /*!< ��֧��,�޷�ʹ��JPEG����ģ����ؽӿ� */
    OPDEVSDK_JPEGDEC_HARDWARE       = 1,        /*!< ֧��JPEGӲ������ */
    OPDEVSDK_JPEGDEC_SOFT           = 2,        /*!< ֧��JPEG��������� */
    OPDEVSDK_JPEGDEC_MAX_NUM        = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_JPEGDEC_ABILI_EN;


/**
* @brief JPEG���ݲ���
*/
typedef struct _OPDEVSDK_JPEGDEC_PARAM_ST_
{
    void                    *pBuf;          /*!< JPEGͼ�����뻺���ַ */
    unsigned int            jpegSize;       /*!< JPEGͼ�����ݴ�С����λByte */
    int                     res[12];        /*!< �����ֶ� */
} OPDEVSDK_JPEGDEC_PARAM_ST;


/** 
* @brief            ����һ·JPEG����ͨ��
*
* @param[out]       handle          JPEG������,ָ������
* @param[in]        bSoftEnc        ��������ʶ,FALSE��ʾʹ��Ӳ������;TRUE��ʾʹ���������
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_create(void **handle,int bSoftDec);


/** 
* @brief            JPEG���봦��
*
* @param[in]        handle          JPEG������
* @param[in]        pParam          JPEG��������
* @param[out]       pFrame          �����YUV����������ָ������
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_proc(void *handle,OPDEVSDK_JPEGDEC_PARAM_ST *pParam,OPDEVSDK_VIDEO_FRAME_ST *pFrame);


/** 
* @brief            ����JPEG����ͨ��
*
* @param[in]        handle          JPEG������
* @param[out]       none                        
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegdec_destroy(void *handle);


#ifdef __cplusplus
}
#endif

#endif //_OP_DEVSDK_JPEGDEC_H_

