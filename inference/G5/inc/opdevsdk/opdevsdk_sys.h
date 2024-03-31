/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_sys.h
* @brief        ������ϵͳģ��
*
* @date         2019/2/18
* @version      1.0.0
* @note         ����sys����
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ���ڲ��ӿ�
*
* @date         2020/2/25
* @version      2.0.1
* @note         ����jpeg��������
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
* @brief ����͸��֧�ֵ����link����
*/
#define   OPDEVSDK_SYS_TRANS_DATA_MAX_LINK      (2)

/**
* @brief �豸����������
*/
typedef struct _OPDEVSDK_SYS_ABILITY_ST_
{
    int                             mmSize;             /*!< ���ܿ���MMZ�ڴ�,��λ:KB,�������Ϊ0,��ʾ������ */
    int                             cacheSize;          /*!< ���ܿ���cache MMZ�ڴ�,��λ:KB,�������Ϊ0,��ʾ������ */
    int                             noCacheSize;        /*!< ���ܿ���no cache MMZ�ڴ�,��λ:KB,�������Ϊ0,��ʾ������ */
    OPDEVSDK_JPEGENC_ABILI_EN       jpegEncAbili;       /*!< �豸jpeg��������,����ο�OPDEVSDK_JPEGENC_ABILI_EN */
    OPDEVSDK_JPEGDEC_ABILI_EN       jpegDecAbili;       /*!< �豸jpeg��������,����ο�OPDEVSDK_JPEGDEC_ABILI_EN */
    OPDEVSDK_IMG_SCALE_ABILI_EN     scaleAbili;         /*!< �豸ͼ����������,����ο�OPDEVSDK_IMG_SCALE_ABILI_EN */
    OPDEVSDK_VIN_ABILITY_ST         vinAbili;           /*!< �豸����ͨ������,����ο�OPDEVSDK_SYS_VIN_ABILITY_ST */
    int                             res[12];            /*!< Ԥ���ֶ� */
}OPDEVSDK_SYS_ABILITY_ST;


/**
* @brief ������汾��Ϣ
*/
typedef struct _OPDEVSDK_SYS_VERSION_ST_
{
    char        version[OPDEVSDK_MEDIADRV_MAX_CHAR_NUM];     /*!< �汾��Ϣ�ַ��� */
}OPDEVSDK_SYS_VERSION_ST;


/**
* @brief ͸��������Ϣ,��Ҫ�����û�ҵ�����ý�����ĳЩ����ͨ��,��˫��Լ����Э�鼴�ɡ�
*/
typedef struct _OPDEVSDK_SYS_COM_DATA_ST_
{
    int                     chan;                   /*!< ǰ��ͨ���� */               
    unsigned long long      timeStamp;              /*!< ʱ���,��λ���� */
    unsigned short          version;                /*!< �汾��Ϣ,���ⲿͨ�Ŷ��� */
    const void              *send;                  /*!< ���ͻ��� */
    int                     sendSize;               /*!< �������ݴ�С */
    void                    *recv;                  /*!< �������ݻ���,����NULL */
    int                     recvMaxSize;            /*!< �������ݻ�������,��recv���ʹ�� */
    int                     *recvSize;              /*!< ʵ�ʽ������ݴ�С,��recv���ʹ�� */
    int                     timeout;                /*!< ��ʱʱ��,-1:����,0:������,����0:��ʱʱ��,��λ���� */
}OPDEVSDK_SYS_COM_DATA_ST;


/** 
* @brief            ����������ʼ��
*
* @param[in]        none
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_init(void);


/** 
* @brief            ���������ȥ��ʼ��
*
* @param[in]        none
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_deinit(void);


/** 
* @brief            ��ȡ�������������
*
* @param[in]        none
* @param[out]       pAbili          ָ���豸������������ָ��
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getAbility(OPDEVSDK_SYS_ABILITY_ST *pAbili);


/** 
* @brief            ��ȡ���������汾��Ϣ
*
* @param[in]        none
* @param[out]       pVersion        ���������汾
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getVersion(OPDEVSDK_SYS_VERSION_ST *pVersion);


/** 
* @brief            ��ȡϵͳʱ��
*
* @param[in]        none
* @param[out]       pTime           ϵͳʱ��
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_getGlobalTime(OPDEVSDK_MEDIADRV_TIME_ST *pTime);


/** 
* @brief            ��ȡ����ʱ���
*
* @param[in]        none
* @param[out]       none
* 
* @return           ���غ���ʱ���
*/
OPDEVSDK_EXPORT unsigned long long opdevsdk_sys_getMs(void);


/** 
* @brief            ��ȡ΢��ʱ���
*
* @param[in]        none
* @param[out]       none
* 
* @return           ����΢��ʱ���
*/
OPDEVSDK_EXPORT unsigned long long opdevsdk_sys_getUs(void);


/** 
* @brief            ��������͸����·ͨ��
*
* @param[in]        none
* @param[out]       handle         ͸����·�����ָ������
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_createTransLink(void **handle);


/** 
* @brief            ��������͸����·
*
* @param[in]        handle         ͸����·���
* @param[out]       none
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_destroyTransLink(void *handle);


/** 
* @brief            ����������Ϣ
*
* @param[in]        handle         ͸����·���
* @param[in/out]    pData          �������������
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_sys_transLink(void *handle, OPDEVSDK_SYS_COM_DATA_ST *pData);


#ifdef __cplusplus
}
#endif

#endif //_OPDEVSDK_SYS_H


