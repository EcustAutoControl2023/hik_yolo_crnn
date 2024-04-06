/**@file     custom_YoloV5_s6_Region_layer.h
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Define interface and struct of YoloV5_s6_Region layer
 * 
 * @author   
 * @date     
 * @version  V1.5.0
 * 
 * @note     
 * @warning  
 */
#ifndef _CUSTOM_YOLOV5_S6_REGION_LAYER_H_
#define _CUSTOM_YOLOV5_S6_REGION_LAYER_H_

#include <float.h>
#include <string.h>
#include <stdio.h>
#include "custom_callback.h"

#ifdef __cplusplus
extern "C" {
#endif

// the definition of OPDEVSDK_YOLOV5_S6_REGION_MODEL_T
typedef struct _OPDEVSDK_YOLOV5_S6_REGION_MODEL_T_
{
    // hyperparams
    int    num_anchors;
    int    num_classes;
    float    conf_thresh;
    float    nms_thresh;
    // params
} OPDEVSDK_YOLOV5_S6_REGION_MODEL_T;

// the definition of OPDEVSDK_YOLOV5_S6_REGION_LAYER_T
typedef struct _OPDEVSDK_YOLOV5_S6_REGION_LAYER_T_
{
    // model handle
    OPDEVSDK_YOLOV5_S6_REGION_MODEL_T       *model;
    // scratch memory
    void *scratch_memory;
    int   scratch_memory_size;
} OPDEVSDK_YOLOV5_S6_REGION_LAYER_T;

/**@fn         CUSTOM_YOLOV5_S6_REGION_reshape
*  @brief      compute the shape of the outblob of yolo layer
*  @param[i/o] void                       *handle   handle of yolo layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of yolo layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_reshape(void                      *handle,
                            OPDEVSDK_HIKFLOW_LDATA_ST *ld);

/**@fn         CUSTOM_YOLOV5_S6_REGION_GetModelMemsize
*  @brief      get the memory size of YoloV5_s6_Region layer for model creation
*  @param[in]  const char                   *hyperparams hyperparams of YoloV5_s6_Region layer
               const char                   *param_blobs param_blobs of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld          informations of YoloV5_s6_Region layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory size and attribution needed for current layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_GetModelMemsize(const char                 *hyperparams,
                                    const char                 *param_blobs,
                                    OPDEVSDK_HIKFLOW_LMODEL_ST *ld,
                                    OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         CUSTOM_YOLOV5_S6_REGION_CreateModel
*  @brief      create model handle of YoloV5_s6_Region layer
*  @param[in]  const char                   *hyperparams  hyperparams of YoloV5_s6_Region layer
               const char                   *param_blobs  param_blobs of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LMODEL_ST   *ld           informations  of YoloV5_s6_Region layer
               OPDEVSDK_HKA_BUF_ST           mem_buf      memory resource
*  @param[out] void                        **handle       memory size and attribution needed for YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_CreateModel(const char                 *hyperparams,
                                const char                 *param_blobs,
                                OPDEVSDK_HIKFLOW_LMODEL_ST *ld,
                                OPDEVSDK_HKA_BUF_ST         mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                                void                      **handle);

/**@fn         CUSTOM_YOLOV5_S6_REGION_GetMemsize
*  @brief      get the memory size of YoloV5_s6_Region layer for handle creation
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of YoloV5_s6_Region layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST     mem_tab  memory size and attribution needed for YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_GetMemsize(OPDEVSDK_HIKFLOW_LDATA_ST   *ld,
                               OPDEVSDK_HKA_MEM_TAB_ST      mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         CUSTOM_YOLOV5_S6_REGION_Create
*  @brief      create layer handle of YoloV5_s6_Region layer
*  @param[in]  OPDEVSDK_HIKFLOW_LDATA_ST   *ld       informations of YoloV5_s6_Region layer
               OPDEVSDK_HKA_BUF_ST          mem_buf  memory resource
*  @param[out] void                       **handle   handle of YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_Create(OPDEVSDK_HIKFLOW_LDATA_ST *ld,
                           OPDEVSDK_HKA_BUF_ST        mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                           void                     **handle);

/**@fn         CUSTOM_YOLOV5_S6_REGION_Forward
*  @brief      do net forward of YoloV5_s6_Region layer
*  @param[i/o] void                       *handle   handle of YoloV5_s6_Region layer
               OPDEVSDK_HIKFLOW_LDATA_ST  *ld       informations of YoloV5_s6_Region layer
*  @return     HRESULT
*/
int CUSTOM_YOLOV5_S6_REGION_Forward(void                      *handle,
                            OPDEVSDK_HIKFLOW_LDATA_ST *ld);

#ifdef __cplusplus
}
#endif

#endif
