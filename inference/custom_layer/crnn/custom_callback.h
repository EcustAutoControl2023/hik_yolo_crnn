/**@file     custom_callback.h
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Defines and interfaces of custom layer
 * 
 * @author   
 * @date     
 * @version  V1.5.0
 * 
 * @note     
 * @warning  
 */
#include "opdevsdk_hka_types.h"
#include "opdevsdk_hikflow_custom.h"

#define HIKFLOW_STS_OK                   (0)                                

#define HIKFLOW_STS_ERR_CUS_LAYER_BASE         			(0x86010000)
#define HIKFLOW_STS_ERR_CUS_LAYER_NULL_PTR				(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 1)
#define HIKFLOW_STS_ERR_CUS_LAYER_RESHAPE_FAILED		(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 2)
#define HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_NULL_PTR	(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 3)
#define HIKFLOW_STS_ERR_CUS_LAYER_HYPER_PARAMS_FAILED	(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 4)
#define HIKFLOW_STS_ERR_CUS_LAYER_ALLOC_ERROR			(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 5)
#define HIKFLOW_STS_ERR_CUS_LAYER_FORWARD_FAILED		(HIKFLOW_STS_ERR_CUS_LAYER_BASE + 6)

#include "custom_YoloV5_s6_Region_layer.h"

//#define DEBUG_CUSTOM_LAYER
#ifdef DEBUG_CUSTOM_LAYER
#define CHECK_ERROR(sts, str, ret)                                                         \
{                                                                                          \
    if (sts)                                                                               \
    {                                                                                      \
        printf("File: %s, Line: %05d: %s\n", __FILE__, __LINE__, (str));                   \
        return (ret);                                                                      \
    }                                                                                      \
}
#define HKA_CHECK_ERROR(flag, sts)    \
{                                     \
    if (flag)                         \
    {                                 \
        return (sts);                 \
    }                                 \
}
#define LOG_INFO  printf
#else
#define CHECK_ERROR(sts, str, ret)    \
{                                     \
    if (sts)                         \
    {                                 \
        return (ret);                 \
    }                                 \
}
#define HKA_CHECK_ERROR(flag, sts)    \
{                                     \
    if (flag)                         \
    {                                 \
        return (sts);                 \
    }                                 \
}
#define LOG_INFO(...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**@fn         Custom_Layer_GetModelMemsize
*  @brief      get the memory size of custom layers for model creation
*  @param[in]  const char                   *hyperparams  hyperparams of current layer
               const char                   *param_blobs  param_blobs of current layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab      memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_GetModelMemsize(const char                  *hyperparams,
                                 const char                  *param_blobs,
                                 OPDEVSDK_HIKFLOW_LMODEL_ST  *ld,
                                 OPDEVSDK_HKA_MEM_TAB_ST      mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         Custom_Layer_CreateModel
*  @brief      create model handle for custom layers
*  @param[in]  const char                   *hyperparams  hyperparams of current layer
               const char                   *param_blobs  param_blobs of current layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations of current layer
               OPDEVSDK_HKA_BUF_ST           mem_buf      memory resource
*  @param[out] void                        **handle       memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_CreateModel(const char                 *hyperparams,
                             const char                 *param_blobs,
                             OPDEVSDK_HIKFLOW_LMODEL_ST *ld,
                             OPDEVSDK_HKA_BUF_ST         mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                             void                      **handle);

/**@fn         Custom_Layer_GetMemsize
*  @brief      get the memory size of current layer for handle creation
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST      mem_tab  memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_GetMemsize(OPDEVSDK_HIKFLOW_LDATA_ST  *ld,
                            OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         Custom_Layer_Create
*  @brief      create layer handle of current layer
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of current layer
               OPDEVSDK_HKA_BUF_ST          mem_buf  memory resource
*  @param[out] void                       **handle   handle of current layer
*  @return     HRESULT
*/
int Custom_Layer_Create(OPDEVSDK_HIKFLOW_LDATA_ST  *ld,
                        OPDEVSDK_HKA_BUF_ST         mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                        void                      **handle);

/**@fn         Custom_Layer_reshape
*  @brief      compute the shape of the tops of current layer
*  @param[i/o] void                       *handle    handle of current layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld        informations of current layer
*  @return     HRESULT
*/
int Custom_Layer_Reshape(void                       *handle,
                         OPDEVSDK_HIKFLOW_LDATA_ST  *ld);

/**@fn         Custom_Layer_Forward
*  @brief      do net forward of current layer
*  @param[i/o] void                       *handle    handle of current layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld        informations of current layer
*  @return     HRESULT
*/
int Custom_Layer_Forward(void                      *handle,
                         OPDEVSDK_HIKFLOW_LDATA_ST *ld);


#ifdef __cplusplus
}
#endif