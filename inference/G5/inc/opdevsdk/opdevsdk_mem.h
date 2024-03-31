/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_mem.h
* @brief        基础服务库内存管理模块
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加mem内存接口
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口 
*
* @date         2020/2/19
* @version      2.0.1
* @note         增加group ID(vb pool group)相关描述,
*               group ID可用于MSCALE模块,用于一进多出视频图像缩放
*****************************************************************************/


#ifndef _OPDEVSDK_MEM_H_
#define _OPDEVSDK_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "opdevsdk_mediadrv_comm.h"

#define OPDEVSDK_MEM_MAX_POOL_NUM       (8)        /*!< 内存管理模块支持的单个GROUP最大缓存池POOL数量 */


/**
* @brief VB POOL缓存池描述
*/
typedef struct _OPDEVSDK_MEM_VB_POOL_ST_
{
    char        name[OPDEVSDK_MEDIADRV_MAX_CHAR_NUM];       /*!< 缓存池名称 */
    int         poolId;                                     /*!< 缓存池Id */
    int         blkCnt;                                     /*!< 缓存池可容纳缓存块数量 */
    int         blkSize;                                    /*!< 单个缓存块大小 ，单位Byte*/
} OPDEVSDK_MEM_VB_POOL_ST;


/**
* @brief VB GROUP缓存池组描述
*/
typedef struct _OPDEVSDK_MEM_VB_POOL_GRP_ST_
{
    int                             poolCnt;                                /*!< 缓存池数量 */
    OPDEVSDK_MEM_VB_POOL_ST         pool[OPDEVSDK_MEM_MAX_POOL_NUM];        /*!< 所有缓存池描述 */
} OPDEVSDK_MEM_VB_POOL_GRP_ST;


/** 
* @brief            申请一块物理地址连续的内存,支持cash
*
* @param[out]       pPhyAddr        物理地址
* @param[out]       pVirAddr        虚拟地址
* @param[in]        pStrMm          内存块名称字符串
* @param[in]        len             待申请缓存大小
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_allocCache(void** pPhyAddr,void** pVirAddr,const char* pStrMm, unsigned int len);


/** 
* @brief            申请一块物理地址连续的内存,不支持cash
*
* @param[out]       pPhyAddr        物理地址
* @param[out]       pVirAddr        虚拟地址
* @param[in]        pStrMm          内存块名称字符串
* @param[in]        len             待申请缓存大小
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_allocNocache(void** pPhyAddr,void** pVirAddr,const char* pStrMm, unsigned int len);


/** 
* @brief            释放内存
*
* @param[in]        pPhyAddr        物理地址
* @param[in]        pVirAddr        虚拟地址
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_free(void* pPhyAddr, void* pVirAddr);


/** 
* @brief            刷新cash数据到DDR内存并使cash数据无效
*
* @param[in]        pPhyAddr        物理地址
* @param[in]        pVirAddr        虚拟地址
* @param[in]        len             待刷新缓存数据大小
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_flushCache(void* pPhyAddr, void* pVirAddr, unsigned int len);


/** 
* @brief            创建一个缓存池组(pool group)
*
* @param[in]        pInput          vb pool group输入参数
* @param[out]       none
*
* @return           成功返回非负值group id,其它返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_createVbPoolGrp(OPDEVSDK_MEM_VB_POOL_GRP_ST *pInput);


/** 
* @brief            获取group id相对应的缓存池组描述
*
* @param[in]        grpId           缓存池组ID
* @param[out]       pOutput         缓存池组描述
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getVbPoolGrpInfo(int grpId,OPDEVSDK_MEM_VB_POOL_GRP_ST *pOutput);


/** 
* @brief            销毁一个缓存池组(pool group)
*
* @param[in]        grpId          缓存池组ID
* @param[out]       none
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_destroyVbPoolGrp(int grpId);


/** 
* @brief            从vb pool中获取一个缓存块
*
* @param[in]        poolId          缓存池ID                          
* @param[in]        blkSize         缓存块大小 
* @param[out]       none
*
* @return           成功返回非负值block id,其它返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getBlkByVbPool(int poolId,int blkSize);


/** 
* @brief            通过block ID获取缓存池POOL ID
*
* @param[in]        blkId           缓存块ID
* @param[out]       none
*
* @return           成功返回非负值pool id,其它返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getVbPoolByBlk(int blkId);


/** 
* @brief            获取缓存块的物理地址
*
* @param[in]        blkId           缓存块ID
* @param[out]       pPhyAddr        物理地址
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_getBlkPhyAddr(int blkId,void **pPhyAddr);


/** 
* @brief            映射物理地址到虚拟地址,非cache属性 
*
* @param[in]        phyAddr         物理地址
* @param[in]        size            缓存大小
* @param[out]       none
*
* @return           成功返回有效地址,失败返回无效地址0
*/
OPDEVSDK_EXPORT void* opdevsdk_mem_map(void* phyAddr,unsigned int size);


/** 
* @brief            映射物理地址到虚拟地址,cache属性 
*
* @param[in]        phyAddr         物理地址
* @param[in]        size            缓存大小
* @param[out]       none
*
* @return           成功返回有效地址,失败返回无效地址0
*/
OPDEVSDK_EXPORT void* opdevsdk_mem_mapCache(void* phyAddr,unsigned int size);


/** 
* @brief            释放映射的虚拟地址
*
* @param[in]        pVirAddr        虚拟地址
* @param[in]        size            缓存大小
* @param[out]       none
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_unmap(void *pVirAddr,unsigned int size);


/** 
* @brief            释放缓存块
* @note             与opdevsdk_mem_getBlkByVbPool配套使用
*
* @param[in]        blkId           缓存块ID
* @param[out]       none
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_mem_releaseBlk(int blkId);


#ifdef __cplusplus
}
#endif

#endif

