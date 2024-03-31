/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mem.h
* @brief        ����������ڴ����ģ��
*
* @date         2019/2/18
* @version      1.0.0
* @note         ����mem�ڴ�ӿ�
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ� 
*
* @date         2020/2/19
* @version      2.0.1
* @note         ����group ID(vb pool group)�������,
*               group ID������MSCALEģ��,����һ�������Ƶͼ������
*****************************************************************************/


#ifndef _OPDEVSDK_MEM_H_
#define _OPDEVSDK_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "opdevsdk_mediadrv_comm.h"

#define OPDEVSDK_MEM_MAX_POOL_NUM       (8)        /*!< �ڴ����ģ��֧�ֵĵ���GROUP��󻺴��POOL���� */


/**
* @brief VB POOL���������
*/
typedef struct _OPDEVSDK_MEM_VB_POOL_ST_
{
    char        name[OPDEVSDK_MEDIADRV_MAX_CHAR_NUM];       /*!< ��������� */
    int         poolId;                                     /*!< �����Id */
    int         blkCnt;                                     /*!< ����ؿ����ɻ�������� */
    int         blkSize;                                    /*!< ����������С ����λByte*/
} OPDEVSDK_MEM_VB_POOL_ST;


/**
* @brief VB GROUP�����������
*/
typedef struct _OPDEVSDK_MEM_VB_POOL_GRP_ST_
{
    int                             poolCnt;                                /*!< ��������� */
    OPDEVSDK_MEM_VB_POOL_ST         pool[OPDEVSDK_MEM_MAX_POOL_NUM];        /*!< ���л�������� */
} OPDEVSDK_MEM_VB_POOL_GRP_ST;


/** 
* @brief            ����һ�������ַ�������ڴ�,֧��cash
*
* @param[out]       pPhyAddr        �����ַ
* @param[out]       pVirAddr        �����ַ
* @param[in]        pStrMm          �ڴ�������ַ���
* @param[in]        len             �����뻺���С
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_allocCache(void** pPhyAddr,void** pVirAddr,const char* pStrMm, unsigned int len);


/** 
* @brief            ����һ�������ַ�������ڴ�,��֧��cash
*
* @param[out]       pPhyAddr        �����ַ
* @param[out]       pVirAddr        �����ַ
* @param[in]        pStrMm          �ڴ�������ַ���
* @param[in]        len             �����뻺���С
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_allocNocache(void** pPhyAddr,void** pVirAddr,const char* pStrMm, unsigned int len);


/** 
* @brief            �ͷ��ڴ�
*
* @param[in]        pPhyAddr        �����ַ
* @param[in]        pVirAddr        �����ַ
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_free(void* pPhyAddr, void* pVirAddr);


/** 
* @brief            ˢ��cash���ݵ�DDR�ڴ沢ʹcash������Ч
*
* @param[in]        pPhyAddr        �����ַ
* @param[in]        pVirAddr        �����ַ
* @param[in]        len             ��ˢ�»������ݴ�С
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_flushCache(void* pPhyAddr, void* pVirAddr, unsigned int len);


/** 
* @brief            ����һ���������(pool group)
*
* @param[in]        pInput          vb pool group�������
* @param[out]       none
*
* @return           �ɹ����طǸ�ֵgroup id,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_createVbPoolGrp(OPDEVSDK_MEM_VB_POOL_GRP_ST *pInput);


/** 
* @brief            ��ȡgroup id���Ӧ�Ļ����������
*
* @param[in]        grpId           �������ID
* @param[out]       pOutput         �����������
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getVbPoolGrpInfo(int grpId,OPDEVSDK_MEM_VB_POOL_GRP_ST *pOutput);


/** 
* @brief            ����һ���������(pool group)
*
* @param[in]        grpId          �������ID
* @param[out]       none
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_destroyVbPoolGrp(int grpId);


/** 
* @brief            ��vb pool�л�ȡһ�������
*
* @param[in]        poolId          �����ID                          
* @param[in]        blkSize         ������С 
* @param[out]       none
*
* @return           �ɹ����طǸ�ֵblock id,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getBlkByVbPool(int poolId,int blkSize);


/** 
* @brief            ͨ��block ID��ȡ�����POOL ID
*
* @param[in]        blkId           �����ID
* @param[out]       none
*
* @return           �ɹ����طǸ�ֵpool id,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getVbPoolByBlk(int blkId);


/** 
* @brief            ��ȡ�����������ַ
*
* @param[in]        blkId           �����ID
* @param[out]       pPhyAddr        �����ַ
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getBlkPhyAddr(int blkId,void **pPhyAddr);


/** 
* @brief            ӳ�������ַ�������ַ,��cache���� 
*
* @param[in]        phyAddr         �����ַ
* @param[in]        size            �����С
* @param[out]       none
*
* @return           �ɹ�������Ч��ַ,ʧ�ܷ�����Ч��ַ0
*/
OPDEVSDK_EXPORT void* opdevsdk_mem_map(void* phyAddr,unsigned int size);


/** 
* @brief            ӳ�������ַ�������ַ,cache���� 
*
* @param[in]        phyAddr         �����ַ
* @param[in]        size            �����С
* @param[out]       none
*
* @return           �ɹ�������Ч��ַ,ʧ�ܷ�����Ч��ַ0
*/
OPDEVSDK_EXPORT void* opdevsdk_mem_mapCache(void* phyAddr,unsigned int size);


/** 
* @brief            �ͷ�ӳ��������ַ
*
* @param[in]        pVirAddr        �����ַ
* @param[in]        size            �����С
* @param[out]       none
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_unmap(void *pVirAddr,unsigned int size);


/** 
* @brief            �ͷŻ����
* @note             ��opdevsdk_mem_getBlkByVbPool����ʹ��
*
* @param[in]        blkId           �����ID
* @param[out]       none
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_mem_releaseBlk(int blkId);


#ifdef __cplusplus
}
#endif

#endif

