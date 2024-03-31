/***************************************************************************
* note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_hikflow_dev_man.h
* @brief        Unified memory function for G5 platform
*
* @author       lixiaogang10
*
* @date         2021/3/12
* @version      1.0.0
*
*****************************************************************************/

#ifndef _OPDEVSDK_HIKFLOW_DEV_MAN_H_
#define _OPDEVSDK_HIKFLOW_DEV_MAN_H_

#include <stdio.h> 
#include <string.h>
#include "opdevsdk_hka_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief choose 64 bits or 32 bits
*/
#if defined(ARM_64)
typedef unsigned long long                 PTR_VOID;
#else
typedef unsigned int                       PTR_VOID;
#endif

/** 
* @brief        alloc_memory_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab                            
* 
* @return      status
* @see         
*/ 
static int alloc_memory_vca(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab);

/** 
* @brief        alloc_mem_tab_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab    
*              int                           num                        
* 
* @return      status
* @see         
*/ 
int opdevsdk_hikflow_Device_Alloc_Memtab(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab, int num);

/** 
* @brief        free_memory_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab                            
* 
* @return      status
* @see         
*/ 
static int free_memory_vca(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab);

/** 
* @brief        free_mem_tab_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab
*              int                           num                            
* 
* @return      void
* @see         
*/
void opdevsdk_hikflow_Device_Free_Memtab(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab, int num);

#ifdef __cplusplus
}
#endif

#endif