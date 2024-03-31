/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_isp.h
* @brief        ���������ISPģ��
*
* @date         2020/3/31
* @version      2.0.0
* @note         ����luma/expose��ؽӿ�.
*****************************************************************************/

#ifndef _OPDEVSDK_ISP_H_
#define _OPDEVSDK_ISP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"

#define OPDEVSDK_ISP_MAX_TARGET_NUM         (128)       /*!< ISPģ��֧�ֵ����Ŀ������ */

/**
* @brief �ع���Ϣ
*/
typedef struct _OPDEVSDK_ISP_EXPOSE_INFO_ST_
{
    int                 enable;         /*!< �ع�ʹ��,OP_FALSE-�ر�,OP_TRUE-���� */
    unsigned int        refBright;      /*!< �ع����ȵȼ�,��Χ[0,100] */
    unsigned int        time;           /*!< �ع����ʱ��,��Χ[1,60],��λ:���� */
} OPDEVSDK_ISP_EXPOSE_INFO_ST;


/**
* @brief Ŀ��������Ϣ
*/
typedef struct _OPDEVSDK_ISP_LUMA_INFO_ST_
{
    unsigned int        tgtNum;         /*!< Ŀ������ */
    unsigned int        *lumaValue;     /*!< ����Ŀ��ƽ������,ָ������,���ȷ�Χ[0,255] */
    unsigned int        *score;         /*!< ����Ŀ��ƽ�����֣�ָ������,���ַ�Χ[0,100] */
} OPDEVSDK_ISP_LUMA_INFO_ST;


/** 
* @brief            ���õ�ǰͨ��Ŀ��������Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pLuma           Ŀ��������Ϣ��ָ������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_isp_setLuma(int chan,OPDEVSDK_ISP_LUMA_INFO_ST *pLuma);


/** 
* @brief            �����ع���Ʋ���
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pExpose         �����ع���Ϣ��ָ������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_isp_setExpo(int chan,OPDEVSDK_ISP_EXPOSE_INFO_ST *pExpose);

/** 
* @brief           isp����ȼ���ȡ
*
* @param[in]        chan            ͨ��id
* @param[out]       level           isp����ȼ�
*                   
* @return           0 �ɹ�  ��0ʧ�ܡ�
*/
OPDEVSDK_EXPORT int opdevsdk_isp_getGainLevel(int chan, int *level);

/** 
* @brief           isp͸��ȼ���ȡ
*
* @param[in]        chan            ͨ��id
* @param[out]       level           isp͸��ȼ�
*                   
* @return           0 �ɹ�  ��0ʧ�ܡ�
*/
OPDEVSDK_EXPORT int opdevsdk_isp_getFogLevel(int chan, int *level);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_ISP_H_ */


