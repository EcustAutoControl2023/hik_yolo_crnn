
/***************************************************************************
* note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_hikflow_dev_man.h
* @brief        Unified memory function for H7 platform
*
* @author       lixiaogang10
*
* @date         2021/3/12
* @version      1.0.0
*
*****************************************************************************/


#include <stdio.h> 
#include "opdevsdk_hikflow_dev_man.h"
#include "demo_comm.h"
/** 
* @brief        alloc_memory_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab                            
* 
* @return      status
* @see         
*/ 
static int alloc_memory_vca(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab)
{
    int            ret   = 0;
    unsigned long long            u64PhyAddr = 0;
    void          *ppVirAddr  = NULL;
    int               align_size = 0;                      ///< aligned memory size

    void           *raw_malloc_ptr = NULL;                 ///< malloc original memory start addr
    size_t          align_off = 0;                         ///< align offset of unaligned memory

    if (mem_tab->alignment > 1)                            ///< necessary to align
    {
        align_size = (int)mem_tab->alignment + mem_tab->size + 2 * sizeof(void *);
    }
    else
    {
        return -1;                         ///< unnecessary to align
    }

    switch (mem_tab->space)
    {
        case OPDEVSDK_HKA_MEM_SPACE_NOCACHED:                ///< mmz without cache
        case OPDEVSDK_HKA_MEM_SPACE_NOCACHED_PRIOPITY:
        {
            ret = opdevsdk_mem_allocNocache((void *)&u64PhyAddr, (void **)&ppVirAddr, (const char *)"mm_no_cache", align_size);
            if (0 != ret)
            {
                DEMOPRT("error: alloc_memory  allocNocache failed\n");
                return -2;
            }

            ///< clear allocated memory
            memset((void *)ppVirAddr, 0, align_size);

            if (mem_tab->alignment > 1)                      ///< necessary to align
            {
                ///< available virtual addr
                mem_tab->base = (void*)(((size_t)ppVirAddr + (size_t)mem_tab->alignment + 2 * sizeof(void *))
                    & (~((size_t)mem_tab->alignment - 1)));

                ///< record virtual addr to free
                ((void **)mem_tab->base)[-1] = (void *)ppVirAddr;

                ///< record physical addr to free
                ((void **)mem_tab->base)[-2] = (void *)(PTR_VOID)u64PhyAddr;

                ///< calculate offset of aligned physical addr
                align_off = (size_t)mem_tab->base - (size_t)ppVirAddr;
                mem_tab->phy_base = (void*)(PTR_VOID)(u64PhyAddr + align_off);
            }

            return 0;
        }
        
        case OPDEVSDK_HKA_MEM_SPACE_CACHED: ///< mmz with cache
        case OPDEVSDK_HKA_MEM_SPACE_CACHED_PRIOPITY: ///< mmz with cache
        {
            ret = opdevsdk_mem_allocCache((void *)&u64PhyAddr, (void **)&ppVirAddr, (const char *)"mm_with_cache", align_size);
            if (0 != ret)
            {
                DEMOPRT("alloc_memory  CACHED_MEM failed\n");
                return -3;
            }

            ///< clear allocated memory
            memset((void *)ppVirAddr, 0, align_size);

            if (mem_tab->alignment > 1)///< necessary to align
            {
                ///< available virtual addr
                mem_tab->base = (void*)(((size_t)ppVirAddr + (size_t)mem_tab->alignment + 2 * sizeof(void *))
                    & (~((size_t)mem_tab->alignment - 1)));

                ///< record virtual addr to free
                ((void **)mem_tab->base)[-1] = (void *)ppVirAddr;

                ///< record physical addr to free
                ((void **)mem_tab->base)[-2] = (void *)(PTR_VOID)u64PhyAddr;

                ///< calculate offset of aligned physical addr
                align_off = (size_t)mem_tab->base - (size_t)ppVirAddr;
                mem_tab->phy_base = (void*)(PTR_VOID)(u64PhyAddr + align_off);
            }

            return 0;
        }
        
        case OPDEVSDK_HKA_MEM_SPACE_CPU:
        {
			ret = opdevsdk_mem_allocCache((void *)&u64PhyAddr, (void **)&ppVirAddr, (const char *)"mm_with_cache", align_size);
            if (0 != ret)
            {
                DEMOPRT("alloc_memory  ARM_MEM failed\n");
                return -3;
            }

            ///< clear allocated memory
            memset((void *)ppVirAddr, 0, align_size);

            if (mem_tab->alignment > 1)///< necessary to align
            {
                ///< available virtual addr
                mem_tab->base = (void*)(((size_t)ppVirAddr + (size_t)mem_tab->alignment + 2 * sizeof(void *))
                    & (~((size_t)mem_tab->alignment - 1)));

                ///< record virtual addr to free
                ((void **)mem_tab->base)[-1] = (void *)ppVirAddr;

                ///< record physical addr to free
                ((void **)mem_tab->base)[-2] = (void *)(PTR_VOID)u64PhyAddr;

                ///< calculate offset of aligned physical addr
                align_off = (size_t)mem_tab->base - (size_t)ppVirAddr;
                mem_tab->phy_base = (void*)(PTR_VOID)(u64PhyAddr + align_off);
            }

            return 0;
        }
        default:
        {
            return -5;
        }
    }

    return -5;
}

/** 
* @brief        alloc_mem_tab_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab    
*              int                           num                        
* 
* @return      status
* @see         
*/ 
int opdevsdk_hikflow_Device_Alloc_Memtab(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab, int num)
{
    int    ret, i;

    ///< allocate one nnie mem_tab everytime 
    for (i = 0; i < num; i++)
    {
        if (mem_tab[i].size > 0)                            ///< necessary to allocate
        {
            ret = alloc_memory_vca(&mem_tab[i]);
        }
        else                                                ///< unnecessary to allocate
        {
            mem_tab[i].phy_base = NULL;
            mem_tab[i].base = NULL;
        }
        DEMOPRT("tab: %d, memsize: %f , space %d.\n", i, mem_tab[i].size / 1024.0 / 1024.0, mem_tab[i].space);
    }

    return 0;
}


/** 
* @brief        free_memory_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab                            
* 
* @return      status
* @see         
*/ 
static int free_memory_vca(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab)
{
    int    ret = 0;

    if (mem_tab->alignment == 0)
    {
        return -1;
    }

    ///< select free operation in terms of memory location
    switch (mem_tab->space)
    {
        ///< use the some function to free mmz memory regardless of cache or not
        case OPDEVSDK_HKA_MEM_SPACE_CACHED:
        case OPDEVSDK_HKA_MEM_SPACE_NOCACHED:
        case OPDEVSDK_HKA_MEM_SPACE_CACHED_PRIOPITY:
        case OPDEVSDK_HKA_MEM_SPACE_NOCACHED_PRIOPITY:
        {
            //DEMOPRT("mem_tab->space = %d\n", mem_tab->space);
            if (mem_tab->phy_base)
            {
                if (mem_tab->alignment > 1)                       ///< free aligned memory block
                {
                    ret = opdevsdk_mem_free((void *)(((void **)mem_tab->base)[-2]), (void *)((void **)mem_tab->base)[-1]);
                }
                if (ret != 0)
                {
                    DEMOPRT("free_memory_vca failed!\n");
                }

                mem_tab->phy_base = NULL;
                mem_tab->base     = NULL;
                DEMOPRT("free_memory_vca OK!\n");
                return 0;
            }
            return -2;
        }

        ///< free malloc memory block
        case OPDEVSDK_HKA_MEM_SPACE_CPU:
        {
			if (mem_tab->phy_base)
			{
			   if (mem_tab->alignment > 1)						 ///< free aligned memory block
			   {
				   ret = opdevsdk_mem_free((void *)(((void **)mem_tab->base)[-2]), (void *)((void **)mem_tab->base)[-1]);
			   }
			   if (ret != 0)
			   {
				   DEMOPRT("free_memory_vca failed!\n");
			   }

			   mem_tab->phy_base = NULL;
			   mem_tab->base	 = NULL;
			   DEMOPRT("free_memory_vca OK!\n");
			   return 0;
			}

        }
        default:
        {
            return -3;
        }
    }
}

/** 
* @brief        free_mem_tab_vca
* @param[in]   OPDEVSDK_HKA_MEM_TAB_ST        *mem_tab
*              int                           num                            
* 
* @return      void
* @see         
*/
void opdevsdk_hikflow_Device_Free_Memtab(OPDEVSDK_HKA_MEM_TAB_ST *mem_tab, int num)
{
    int i;

    for (i = 0; i < num; i++)
    {
        if (mem_tab[i].base || mem_tab[i].phy_base)
        {
            free_memory_vca(&mem_tab[i]);
        }
    }
}
