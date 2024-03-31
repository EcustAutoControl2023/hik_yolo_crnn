/**@file     custom_callback.c
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
#include <string.h>
#include <stdio.h>
#include "custom_callback.h"

/**@fn         Custom_Layer_GetModelMemsize
*  @brief      get the memory size of custom layers for model creation
*  @param[in]  const char                   *hyperparams  hyperparams of current layer
               const char                   *param_blobs  param_blobs of current layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab      memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_GetModelMemsize(const char                 *hyperparams,
                                 const char                 *param_blobs,
                                 OPDEVSDK_HIKFLOW_LMODEL_ST *ld,
                                 OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM])
{
    int hr = 0;
    LOG_INFO("Custom_Layer_GetModelMemsize %s\n", ld->type);
    
    if (0 == strcmp(ld->type, "YoloV5_s6_Region"))
    {
        hr = CUSTOM_YOLOV5_S6_REGION_GetModelMemsize(hyperparams, param_blobs, ld, mem_tab);
    }
    else
    {
        hr = -1;
    }
    return hr;
}

/**@fn         Custom_Layer_CreateModel
*  @brief      create model handle for custom layers
*  @param[in]  const char                   *hyperparams  hyperparams of current layer
               const char                   *param_blobs  param_blobs of current layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations of current layer
               OPDEVSDK_HKA_BUF_ST           mem_buf      memory resource
*  @param[out] void                        **handle       memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_CreateModel(const char                  *hyperparams,
                             const char                  *param_blobs,
                             OPDEVSDK_HIKFLOW_LMODEL_ST  *ld,
                             OPDEVSDK_HKA_BUF_ST          mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                             void                       **handle)
{
    int hr = 0;
    LOG_INFO("Custom_Layer_CreateModel %s\n", ld->type);

    if (0 == strcmp(ld->type, "YoloV5_s6_Region"))
    {
        hr = CUSTOM_YOLOV5_S6_REGION_CreateModel(hyperparams, param_blobs, ld, mem_buf, handle);
    }
    else
    {
        hr = -1;
    }
    return hr;
}

/**@fn         Custom_Layer_GetMemsize
*  @brief      get the memory size of current layer for handle creation
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST      mem_tab  memory size and attribution needed for current layer
*  @return     HRESULT
*/
int Custom_Layer_GetMemsize(OPDEVSDK_HIKFLOW_LDATA_ST  *ld,
                            OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM])
{
    int hr = 0;
    LOG_INFO("Custom_Layer_GetMemsize %s\n", ld->layer_model->type);
    
    if (0 == strcmp(ld->layer_model->type, "YoloV5_s6_Region"))
    {
        hr = CUSTOM_YOLOV5_S6_REGION_GetMemsize(ld, mem_tab);
    }
    else
    {
        hr = -1;
    }
    return hr;
}

/**@fn         Custom_Layer_Create
*  @brief      create layer handle of current layer
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of current layer
               OPDEVSDK_HKA_BUF_ST          mem_buf  memory resource
*  @param[out] void                       **handle   handle of current layer
*  @return     HRESULT
*/
int Custom_Layer_Create(OPDEVSDK_HIKFLOW_LDATA_ST  *ld,
                        OPDEVSDK_HKA_BUF_ST         mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                        void                      **handle)
{
    int hr = 0;
    LOG_INFO("Custom_Layer_Create %s\n", ld->layer_model->type);

    if (0 == strcmp(ld->layer_model->type, "YoloV5_s6_Region"))
    {
        hr = CUSTOM_YOLOV5_S6_REGION_Create(ld, mem_buf, handle);
    }
    else
    {
        hr = -1;
    }
    return hr;
}

/**@fn         Custom_Layer_reshape
*  @brief      compute the shape of the tops of current layer
*  @param[i/o] void                       *handle    handle of current layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld        informations of current layer
*  @return     HRESULT
*/
int Custom_Layer_Reshape(void                       *handle,
                         OPDEVSDK_HIKFLOW_LDATA_ST  *ld)
{
    int hr = 0;
    LOG_INFO("Custom_Layer_Reshape %s\n", ld->layer_model->type);

    if (0 == strcmp(ld->layer_model->type, "YoloV5_s6_Region"))
    {
        // compute the shape of the out blobs.
    }
    else
    {
        hr = -1;
    }
    return 0;
}

/**@fn         Custom_Layer_Forward
*  @brief      do net forward of current layer
*  @param[i/o] void                       *handle    handle of current layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld        informations of current layer
*  @return     HRESULT
*/
int Custom_Layer_Forward(void                       *handle,
                         OPDEVSDK_HIKFLOW_LDATA_ST  *ld)
{
    int hr = 0;
    LOG_INFO("Custom_Layer_Forward %s\n", ld->layer_model->type);
    
    if (0 == strcmp(ld->layer_model->type, "YoloV5_s6_Region"))
    {
        hr = CUSTOM_YOLOV5_S6_REGION_Forward(handle, ld);
    }
    else
    {
        hr = -1;
    }
    return hr;
}