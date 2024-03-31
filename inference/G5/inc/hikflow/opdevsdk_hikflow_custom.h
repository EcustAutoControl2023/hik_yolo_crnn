/**@file     opdevsdk_hikflow_custom.h
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Defines and interfaces of custom layer
 * 
 * @author   chongji huang
 * @date     2019/02/21
 * @version  V0.5.0
 * 
 * @note     Initial Draft
 * @note History:
 * @note     2019/06/13 V1.0.0 mzm modify specification
 * @warning  
 */

#ifndef _OPDEVSDK_HIKFLOW_CUSTOM_H_
#define _OPDEVSDK_HIKFLOW_CUSTOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_hka_types.h"

#define OPDEVSDK_HIKFLOW_LMI_BLOBS     16        ///< max input blobs of one layer(bottom)
#define OPDEVSDK_HIKFLOW_LMO_BLOBS     8         ///< max output blobs of one layer(top)
#define OPDEVSDK_HIKFLOW_MSTRL         128       ///< max length of names

///< max of OPDEVSDK_HIKFLOW_LMI_BLOBS and OPDEVSDK_HIKFLOW_LMO_BLOBS
#define OPDEVSDK_HIKFLOW_MSIEN         (OPDEVSDK_HIKFLOW_LMI_BLOBS > OPDEVSDK_HIKFLOW_LMO_BLOBS ?         \
                                        OPDEVSDK_HIKFLOW_LMI_BLOBS : OPDEVSDK_HIKFLOW_LMO_BLOBS)

///< max links of tops(current layer) and bottoms(other layers)
#define OPDEVSDK_HIKFLOW_MLINK         64

struct _OPDEVSDK_HIKFLOW_LMODEL_ST_;
struct _OPDEVSDK_HIKFLOW_LDATA_ST_;

/**@fn         opdevsdk_hikflow_cl_GetModelMemsize
*  @brief      get the memory size of current layer for model creation 
*  @param[in]  const char                   *hyperparams hyperparams of current layer
               const char                   *param_blobs param_blobs of current layer
               _OPDEVSDK_HIKFLOW_LMODEL_ST_ *ld          other informations  of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory size and attribution needed for current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_GetModelMemsize)(const char                           *hyperparams,
                                                       const char                           *param_blobs,
                                                       struct _OPDEVSDK_HIKFLOW_LMODEL_ST_  *ld,
                                                       OPDEVSDK_HKA_MEM_TAB_ST               mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         opdevsdk_hikflow_cl_CreateModel
*  @brief      create model handle of current layer 
*  @param[in]  const char                   *hyperparams  hyperparams of current layer
               const char                   *param_blobs  param_blobs of current layer
               _OPDEVSDK_HIKFLOW_LMODEL_ST_ *ld other     informations  of current layer
               OPDEVSDK_HKA_BUF_ST           mem_buf      memory resource
*  @param[out] void                        **handle       memory size and attribution needed for current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_CreateModel)(const char                           *hyperparams,
                                                   const char                           *param_blobs,
                                                   struct _OPDEVSDK_HIKFLOW_LMODEL_ST_  *ld,
                                                   OPDEVSDK_HKA_BUF_ST                   mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                                                   void                                **handle);

/**@fn         opdevsdk_hikflow_cl_GetMemsize
*  @brief      get the memory size of current layer for handle creation
*  @param[in]  _OPDEVSDK_HIKFLOW_LDATA_ST_ *ld      other informations of current layer
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST      mem_tab memory size and attribution needed for current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_GetMemsize)(struct _OPDEVSDK_HIKFLOW_LDATA_ST_  *ld,
                                                  OPDEVSDK_HKA_MEM_TAB_ST              mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn        opdevsdk_hikflow_cl_Create
*  @brief     create layer handle of current layer 
*  @param[in]  _OPDEVSDK_HIKFLOW_LDATA_ST_ *ld      other informations of current layer
               OPDEVSDK_HKA_BUF_ST          mem_buf memory resource
*  @param[out] void                       **handle  handle of current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_Create)(struct _OPDEVSDK_HIKFLOW_LDATA_ST_  *ld,
                                              OPDEVSDK_HKA_BUF_ST                  mem_buf[OPDEVSDK_HKA_MEM_TAB_NUM],
                                              void                               **handle);

/**@fn         opdevsdk_hikflow_cl_Forward
*  @brief      do net forward of current layer
*  @param[i/o] void                       *handle    handle of current layer
               _OPDEVSDK_HIKFLOW_LDATA_ST_ *ld       other informations of current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_Forward)(void                                *handle,
                                               struct _OPDEVSDK_HIKFLOW_LDATA_ST_  *ld);

/**@fn         opdevsdk_hikflow_cl_Reshape
*  @brief      compute the shape of the tops of current layer
*  @param[i/o] void                       *handle    handle of current layer
               _OPDEVSDK_HIKFLOW_LDATA_ST_ *ld       other informations of current layer
*  @return     HRESULT
*/
typedef HRESULT (*opdevsdk_hikflow_cl_Reshape)(void                            *handle,
                                                struct _OPDEVSDK_HIKFLOW_LDATA_ST_   *ld);

/**@brief enum of NPU type
*/
typedef enum 
{
    OPDEVSDK_HIKFLOW_NPU_TYPE_0    = 0,
    OPDEVSDK_HIKFLOW_NPU_TYPE_1    = 1,
    
    OPDEVSDK_HIKFLOW_NPU_TYPE_END  = OPDEVSDK_HKA_ENUM_END
} OPDEVSDK_HIKFLOW_NPU_TYPE_EN;


/**@brief output informations of segment or layer
*/
typedef struct
{
    int    seg_id;    ///< segment id
    int    layer_id;  ///< layer id
    int    blob_id;   ///< blob id

} OPDEVSDK_HIKFLOW_LAYER_PIN_ST;


/**@brief set
*/
typedef struct
{
    short ele_num;                            ///< number of element
    short elements[OPDEVSDK_HIKFLOW_MSIEN];   ///< elements

} OPDEVSDK_HIKFLOW_SET_I_ST;

/**@brief struct of layer model
*/
typedef struct _OPDEVSDK_HIKFLOW_LMODEL_ST_
{
    short                                id;                                 ///< layer id
    char                                 name[OPDEVSDK_HIKFLOW_MSTRL];       ///< layer name
    char                                 type[OPDEVSDK_HIKFLOW_MSTRL];       ///< string of layer type
    OPDEVSDK_HIKFLOW_NPU_TYPE_EN         npu_enum_val;                       ///< NPU type(processor type) 
    int                                  model_type;                         ///< enum of model type
    int                                  type_enum_val;                      ///< enum of layer type

    int                                  in_place;                           ///< if current layer is in place

    void                                *model_handle;                       ///< layer model handle

    const char                          *hyperparams;                        ///< layer  hyper parameters
    const char                          *param_blobs;                        ///< layer param blobs

    int                                  input_blobs_num;                    ///< input blobs number(bottom)
    OPDEVSDK_HIKFLOW_SET_I_ST            input_layers_id;                    ///< layer id of bottoms

    ///< the ids(layer id and blob id) of bottoms input to current layer
    OPDEVSDK_HIKFLOW_LAYER_PIN_ST        input_blobs_id[OPDEVSDK_HIKFLOW_LMI_BLOBS];


    int                                  output_blobs_num;                   ///< output blobs number(top)
    int                                  required_layer_num;                 ///< number of top layers
    int                                  outblob_cnt;                        ///< outblob counters

    ///< the ids(layer id and blob id) of tops output to other layers
    OPDEVSDK_HIKFLOW_LAYER_PIN_ST        output_blobs_id[OPDEVSDK_HIKFLOW_MLINK];


    opdevsdk_hikflow_cl_GetModelMemsize  get_model_memsize;                  ///< get model memsize call back
    opdevsdk_hikflow_cl_CreateModel      create_model;                       ///< create_model call back
    opdevsdk_hikflow_cl_GetMemsize       get_memsize;                        ///< get_memsize call back
    opdevsdk_hikflow_cl_Create           create;                             ///< create call back
    opdevsdk_hikflow_cl_Forward          forward;                            ///< forward call back
    opdevsdk_hikflow_cl_Reshape          reshape;                            ///< reshape call back

    char                                 reserved[32];                       ///< reserved bytes

} OPDEVSDK_HIKFLOW_LMODEL_ST;

/**@brief struct of layer data
*/
typedef struct _OPDEVSDK_HIKFLOW_LDATA_ST_
{
    void                            *layer_handle;                                 ///< layer handle
    OPDEVSDK_HIKFLOW_LMODEL_ST      *layer_model;                                  ///< layer model
    
    int                              required_layer_num;                           ///< number of top layers

    int                              output_blobs_num;                             ///< output blobs number(top)
    OPDEVSDK_HKA_BLOB_ST             output_blobs[OPDEVSDK_HIKFLOW_LMO_BLOBS];     ///< output blobs

    int                              input_blobs_num;                              ///< input blobs number(bottom)
    OPDEVSDK_HKA_BLOB_ST            *input_blobs[OPDEVSDK_HIKFLOW_LMI_BLOBS];      ///< input blobs

    char                             reserved[32];                                 ///< reserved bytes

} OPDEVSDK_HIKFLOW_LDATA_ST;

#ifdef __cplusplus
}
#endif

#endif
