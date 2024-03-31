/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mscale.h
* @brief        ������multi-scale(һ���������)ģ��
*
* @date         2020/3/31
* @version      2.0.0
* @note         ����������Ϣ��mscale��ؽӿ�
*               
*****************************************************************************/

#ifndef _OPDEVSDK_MSCALE_H_
#define _OPDEVSDK_MSCALE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_vin.h"


#define OPDEVSDK_MSCALE_MAX_OUT_RES_NUM         (8)     /*!< multi-scale֧�ֵĵ�ͨ�����������ŷֱ������� */ 

/**
* @brief  ͼ��ֱ�����Ϣ
*/
typedef struct _OPDEVSDK_MSCALE_RES_ST_
{
    int     width;          /*!< ͼ����,��λ:����,8���ض��� */
    int     pitch;          /*!< ͼ����,��λ:����,16���ض��� */
    int     height;         /*!< ͼ��߶�,��λ:����,8���ض��� */
} OPDEVSDK_MSCALE_RES_ST;


/**
* @brief  multi-scale��ʼ������������һ�����ͼ����Ϣ
*/
typedef struct _OPDEVSDK_MSCALE_INIT_ST_
{
    OPDEVSDK_MSCALE_RES_ST      srcImg;                                         /*!< ����Դͼ��Ϣ */
    int                         dstImgCnt;                                      /*!< �������Ŀ��ͼ��ֱ������� */
    OPDEVSDK_MSCALE_RES_ST      dstImg[OPDEVSDK_MSCALE_MAX_OUT_RES_NUM];        /*!< �������Ŀ��ͼ����Ϣ */
} OPDEVSDK_MSCALE_INIT_ST;


/** 
* @brief            ����һ·����ͨ��(multi-scale)���
*
* @param[out]       handle          ��������ͨ�������ָ������
* @param[in]        pInput          ��ʼ������
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_create(void** handle, OPDEVSDK_MSCALE_INIT_ST *pInput);


/** 
* @brief            ��VB�������������ͨ�������
* @note             grpId��opdevsdk_mem_createVbPoolGrp��������multi-scale���е��ڴ����
*
* @param[in]        handle          ����ͨ�����
* @param[in]        grpId           VB�������ID
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_bindVbPoolGrp(void *handle,int grpId);


/** 
* @brief            ��VB�������������ͨ��������
*
* @param[in]        handle          ����ͨ�����
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_unbindVbPoolGrp(void *handle);


/** 
* @brief            ��������ͨ��(multi-scale)���
*
* @param[in]        handle          ����ͨ�����
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_destroy(void *handle);


/** 
* @brief            ��������ͨ��
*
* @param[in]        handle          ����ͨ�����
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_start(void *handle);


/** 
* @brief            �ر�����ͨ��
*
* @param[in]        handle          ����ͨ�����
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_stop(void *handle);

/** 
* @brief            ����һ֡VBCͼ�����ݵ�����ͨ��
*
* @param[in]        handle          ����ͨ�����
* @param[in]        vbc             vbc���ݣ�ָ������
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_sendVbc(void *handle,void *vbc);


/** 
* @brief            ����һ֡ͼ�����ݵ�����ͨ��
*
* @param[in]        handle          ����ͨ�����
* @param[in]        pFrame          ͼ������������ָ������
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_sendFrame(void *handle,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            ������ͨ����ȡһָ֡���ֱ��ʵ�ͼ������
*
* @param[in]        handle          ����ͨ�����
* @param[in]        img             ͼ��ֱ�����Ϣ
* @param[out]       pFrame          ͼ������������ָ������
* @param[in]        timeout         ��ʱʱ��,��λ:ms,<-1Ϊ�Ƿ�����,-1Ϊ������ȡ,0Ϊ��������ȡ,����Ϊ���ʱʱ���ȡ
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_getFrame(void *handle,OPDEVSDK_MSCALE_RES_ST img,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame,int timeout);


/** 
* @brief            �ͷ�һ֡ͼ������
*
* @param[in]        handle          ����ͨ�����
* @param[in]        pFrame          ͼ������������ָ������
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_releaseFrame(void *handle,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            ������ͨ����ȡһָ֡���ֱ��ʵ�VBC��Ϣ
* @note             VBC��Ϣ�洢��Ҳ��ͼ����Ϣ
*
* @param[in]        handle          ����ͨ�����
* @param[in]        img             �������ͼ��ֱ�������
* @param[out]       pVbc            VBC����ָ��
* @param[in]        timeout         ��ʱʱ��,��λ:����,<-1Ϊ�Ƿ�����,-1Ϊ������ȡ,0Ϊ��������ȡ,����Ϊ���ʱʱ���ȡ
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_getVbc(void *handle,OPDEVSDK_MSCALE_RES_ST img,void **pVbc,int timeout);


/** 
* @brief            �ͷ�һ֡VBCͼ������
*
* @param[in]        pVbc            VBC����ָ��
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mscale_releaseVbc(void *vbc);


#ifdef __cplusplus
}
#endif


#endif //_OPDEVSDK_MSCALE_H_

