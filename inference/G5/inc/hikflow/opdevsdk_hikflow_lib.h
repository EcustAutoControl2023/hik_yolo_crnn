/**@file     opdevsdk_hikflow_lib.h
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Defines and interfaces of hikflow
 * 
 * @author   chongji huang
 * @date     2019/03/04
 * @version  V0.5.0
 * 
 * @note     Initial Draft
 * @note History:
 * @note     2019/06/13 V1.0.0 mzm modify specification
 * @warning  
 */
#ifndef _OPDEVSDK_HIKFLOW_LIB_H_
#define _OPDEVSDK_HIKFLOW_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "opdevsdk_hka_types.h"

#define OPDEVSDK_HIKFLOW_MAX_IN_BLOB           8           ///< max input blobs when processing
#define OPDEVSDK_HIKFLOW_MAX_OUT_BLOB          32          ///< max output blobs when processing

/**@brief information of input blob
*/
typedef struct
{
    OPDEVSDK_HKA_BLOB_ST         src_blob;           ///< src blob
    OPDEVSDK_HKA_YUV_FORMAT_EN   src_format;         ///< format of src blob(BGR\RGB\GRAY\NV12\YV12\I420)
    int                          valid_roi;          ///< whether use roi or not
    OPDEVSDK_HKA_RECT_F_ST       roi;                ///< region of roi
    char                         res[32];            ///< reserve
}OPDEVSDK_HIKFLOW_IN_BLOB_INFO_ST;

/**@brief information of out blob
*/
typedef struct
{
    int layer_idx;  ///< layer idx of output blob(can be negative, -1 means the last layer)
    int blob_idx;   ///< blob idx of output blob(use 0 when there is only one blob)
}OPDEVSDK_HIKFLOW_OUT_BLOB_INFO_ST;


/**@brief type of handle
*/
typedef enum 
{
    OPDEVSDK_HIKFLOW_MODEL_HANDLE    = 0,         ///< model handle

    OPDEVSDK_HIKFLOW_HANDLE_TYPE_NUM = OPDEVSDK_HKA_ENUM_END
}OPDEVSDK_HIKFLOW_HANDLE_TYPE_EN;

/**@brief struct of handle
*/
typedef struct
{
    OPDEVSDK_HIKFLOW_HANDLE_TYPE_EN   type;       ///< type of handle
    void                             *handle;     ///< handle
}OPDEVSDK_HIKFLOW_HANDLE_ST;

/**@brief struct of hikflow param
*/
typedef struct 
{
    int                               in_blob_num;                                  ///< in blob num
    OPDEVSDK_HIKFLOW_IN_BLOB_INFO_ST  in_blob_param[OPDEVSDK_HIKFLOW_MAX_IN_BLOB];  ///< information of in blobs
    int                               out_blob_num;                                 ///< out blob num
    OPDEVSDK_HIKFLOW_OUT_BLOB_INFO_ST out_blob_info[OPDEVSDK_HIKFLOW_MAX_OUT_BLOB]; ///< information of out blobs

    int                               handle_num;                                   ///< handle num
    OPDEVSDK_HIKFLOW_HANDLE_ST        handle[OPDEVSDK_HKA_HANDLE_NUM];              ///< handle

    void                             *private_param;                                ///< private_param
    char                              res[16];                                      ///< reserve
}OPDEVSDK_HIKFLOW_PARAM_ST;

/**@brief process type
*/
typedef enum
{
    OPDEVSDK_PROC_TYPE_FORWARD,                    ///< forward
    OPDEVSDK_PROC_TYPE_NUM = OPDEVSDK_HKA_ENUM_END
}OPDEVSDK_HIKFLOW_PROCTYPE_EN;
//     PROC_TYPE_FORWARD,                    ///< forward
typedef enum
{
    OPDEVSDK_HIKFLOW_CMD_TYPE_PROFILE = 0xf0,                 ///< profile
    OPDEVSDK_HIKFLOW_CMD_TYPE_NUM = OPDEVSDK_HKA_ENUM_END
}OPDEVSDK_HIKFLOW_CONFIG_CMD_TYPE_EN;
typedef enum
{
    OPDEVSDK_HIKFLOW_PROFILE_LEVEL_0 = 0,                ///< close profile
    OPDEVSDK_HIKFLOW_PROFILE_LEVEL_1,                    ///< basic information and timeline
    OPDEVSDK_HIKFLOW_PROFILE_LEVEL_2,                    ///< data dump
    OPDEVSDK_HIKFLOW_PROFILE_LEVEL_NUM = OPDEVSDK_HKA_ENUM_END
}OPDEVSDK_HIKFLOW_PROFILE_LEVEL_TYPE_EN;
typedef struct
{
    OPDEVSDK_HIKFLOW_PROFILE_LEVEL_TYPE_EN  profile_level;                 ///< profile level
    int                                     multi_app_mode;                ///< multiple process running
}OPDEVSDK_HIKFLOW_PROFILE_INFO_ST;

/**@brief in parameters when forward
*/
typedef struct
{
    int                              in_blob_num;                           ///< in blob num
    OPDEVSDK_HIKFLOW_IN_BLOB_INFO_ST in_blob[OPDEVSDK_HIKFLOW_MAX_IN_BLOB]; ///< information of in blobs

    int                              end_layer_idx;                         ///< reserve
    char                             res[64];                               ///< reserve
}OPDEVSDK_HIKFLOW_FORWARD_IN_INFO_ST;

/**@brief out parameter when forward
*/
typedef struct
{
    int                  blob_num;                                   ///< out blob num
    OPDEVSDK_HKA_BLOB_ST output_blob[OPDEVSDK_HIKFLOW_MAX_OUT_BLOB]; ///< out blbo
    char                 res[64];
}OPDEVSDK_HIKFLOW_FORWARD_OUT_INFO_ST;

/**@fn         opdevsdk_hikflow_GetModelMemSize
*  @brief      get the memory size of model
*  @param[in]  OPDEVSDK_HKA_MODEL_INFO_ST   *params_info model info
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory size and attribution needed for model
*  @return     status
*/
int opdevsdk_hikflow_GetModelMemSize(OPDEVSDK_HKA_MODEL_INFO_ST *params_info, 
                                     OPDEVSDK_HKA_MEM_TAB_ST     mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         opdevsdk_hikflow_CreateModel
*  @brief      create model handle
*  @param[in]  OPDEVSDK_HKA_MODEL_INFO_ST   *params_info model info
               OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory resource
*  @param[out] void                        **handle      model handle
*  @return     status
*/
int opdevsdk_hikflow_CreateModel(OPDEVSDK_HKA_MODEL_INFO_ST    *params_info,
                                 OPDEVSDK_HKA_MEM_TAB_ST        mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM],
                                 void                         **handle);

/**@fn         opdevsdk_hikflow_GetMemSize
*  @brief      get the memory size of net
*  @param[in]  OPDEVSDK_HIKFLOW_PARAM_ST    *params_info params_info of net
*  @param[out] OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     memory size and attribution needed for net
*  @return     status
*/
int opdevsdk_hikflow_GetMemSize(OPDEVSDK_HIKFLOW_PARAM_ST *params_info,
                                OPDEVSDK_HKA_MEM_TAB_ST    mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM]);

/**@fn         opdevsdk_hikflow_Create
*  @brief      create net handle
*  @param[in]  OPDEVSDK_HIKFLOW_PARAM_ST    *params_info params_info of net
               OPDEVSDK_HKA_MEM_TAB_ST       mem_tab     resource
*  @param[out] void                        **handle      net handle
*  @return     status
*/
int opdevsdk_hikflow_Create(OPDEVSDK_HIKFLOW_PARAM_ST *params_info,
                            OPDEVSDK_HKA_MEM_TAB_ST    mem_tab[OPDEVSDK_HKA_MEM_TAB_NUM],
                            void                     **handle);

/**@fn         opdevsdk_hikflow_SetConfig
*  @brief      set config for net handle
*  @param[in]  void    *handle   net handle
               int      cmd      cmd type
               void    *cfg      config information(pointer)
               int      cfg_size size of config information
*  @return     status
*/
int opdevsdk_hikflow_SetConfig(void  *handle,
                               OPDEVSDK_HIKFLOW_CONFIG_CMD_TYPE_EN  cmd,
                               void  *cfg,
                               int    cfg_size);

/**@fn         opdevsdk_hikflow_GetConfig
*  @brief      get config for net handle
*  @param[in]  void    *handle   net handle
               int      cmd      cmd type
               int      cfg_size size of config information
*  @param[out] void    *cfg      config information(pointer)
*  @return     status
*/
int opdevsdk_hikflow_GetConfig(void  *handle,
                               int    cmd,
                               void  *cfg,
                               int    cfg_size);

/**@fn         opdevsdk_hikflow_Process
*  @brief      forward
*  @param[in]  void    *handle       net handle
               int      proc_type    process type
               void     in_buf       input buf
               int      in_buf_      size of input buf
               int      out_buf_size size of output buf
*  @param[out] void    *out_buf      output buf
*  @return     status
*/
int opdevsdk_hikflow_Process(void  *handle,
                             int    proc_type,
                             void  *in_buf,
                             int    in_buf_size,
                             void  *out_buf,
                             int    out_buf_size);

/**@fn         opdevsdk_hikflow_Release
*  @brief      release net handle
*  @param[in]  void    *handle       net handle
*  @return     status
*/
int opdevsdk_hikflow_Release(void *handle);

/**@fn         opdevsdk_hikflow_ReleaseModel
*  @brief      release model handle
*  @param[in]  void    *handle       model handle
*  @return     status
*/
int opdevsdk_hikflow_ReleaseModel(void *handle);

/**@fn         opdevsdk_hikflow_GetVersion
*  @brief      get hikflow version
*  @param[i/o] OPDEVSDK_HKA_VERSION_ST *version  String of version information
*  @return     status
*/
int opdevsdk_hikflow_GetVersion(OPDEVSDK_HKA_VERSION_ST *version);

#ifdef __cplusplus
}
#endif

#endif

