/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_vin.h
* @brief        ��������Ƶ����ģ��ӿ�
*
* @date         2019/2/18
* @version      1.0.0
* @note         ����new/put frame��ؽӿ�
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ�,ɾ��״̬ͳ��
*
* @date         2020/2/19
* @version      2.0.1
* @note         ���ӽӿ�������VBC��ؽӿ�
*****************************************************************************/

#ifndef _OPDEVSDK_VIN_H_
#define _OPDEVSDK_VIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_video.h"

#define OPDEVSDK_VIN_CHN_MAX_NUM        (16)        /*!< ���������֧���������ͨ������ */


/**
* @brief ͨ������
*/
typedef struct _OPDEVSDK_VIN_CHN_ST_
{
    int             chan;           /*!< ����ͨ���� */
    int   			fovChn;       	/*!< fovͨ����,��0��ʼ,֧����չͨ�� */    
    int             width;          /*!< ����ͨ�������,��λ:���� */
    int             height;         /*!< ����ͨ�����߶�,��λ:���� */
    float           fps;            /*!< ����ͨ�����֡��,������,��λ:֡/�� */
    int             res[4];         /*!< Ԥ���ֶ� */
}OPDEVSDK_VIN_CHN_ST;

/**
* @brief �豸����ͨ������
*/
typedef struct _OPDEVSDK_VIN_ABILITY_ST_
{
    int                      chnNum;                                /*!< ����ͨ������ */
    OPDEVSDK_VIN_CHN_ST      chnInfo[OPDEVSDK_VIN_CHN_MAX_NUM];     /*!< ÿ������ͨ������ */
}OPDEVSDK_VIN_ABILITY_ST;

/** 
* @brief            ��Ƶ����ģ���ʼ�� 
*
* @param[in]        chan            ǰ��ͨ����
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_init(int chan);


/** 
* @brief            ��Ƶ����ģ��ȥ��ʼ��
*
* @param[in]        none
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_deinit(int chan);


/** 
* @brief            ���û�ȡ��Ƶ��֡��
* @note             ��Ҫָ��ͨ����Ĭ��1
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        frameRate       �趨����Ƶ֡�� 
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_setFrameRate(int chan,OPDEVSDK_VIDEO_FRAME_RATE_EN frameRate);


/** 
* @brief            ��ȡһ֡ǰ��ͼ������
* @note             ��Ҫָ��ͨ����Ĭ��1
*
* @param[in]        chan            ǰ��ͨ����
* @param[in/out]    pFrame          ͼ��������ָ�����ͣ���Ҫָ���ֱ���
* @param[in]        timeout         ��ʱʱ��,��λ:����,timeout<-1Ϊ�Ƿ�����,-1Ϊ������ȡ,0Ϊ��������ȡ,����Ϊ���ʱʱ���ȡ
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_getFrame(int chan,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame,int timeout);


/** 
* @brief            �ͷ�һ֡ͼ������
* @note             ��Ҫָ��ͨ����Ĭ��1,��getFrame�ɶ�ʹ��
*
* @param[in]        chan            ǰ��ͨ����
* @param[in/out]    pFrame          ͼ��������ָ�����ͣ���Ҫָ���ֱ���
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_releaseFrame(int chan,OPDEVSDK_VIDEO_FRAME_INFO_ST *pFrame);


/** 
* @brief            ��ȡһ֡VBCͼ������
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        timeout         ��ʱʱ��,��λ:����,timeout<-1Ϊ�Ƿ�����,-1Ϊ������ȡ,0Ϊ��������ȡ,����Ϊ���ʱʱ���ȡ
* @param[out]       pVbc            ָ��VBC����ָ���ָ��
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_getVbc(int chan,void **pVbc,int timeout);


/** 
* @brief            �ͷ�һ֡VBCͼ������
* @note             ��Ҫָ��ͨ����Ĭ��1,��getVbc�ɶ�ʹ��
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        vbc             ָ��VBC���͵�ָ��
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_vin_releaseVbc(int chan,void *vbc);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_VIN_H_ */


