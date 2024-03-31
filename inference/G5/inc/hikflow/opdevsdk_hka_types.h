/**@file     opdevsdk_hka_tpyes.h
 * @note     2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
 * @brief    Defines of some base data structures
 * 
 * @author   chongji huang
 * @date     2019/02/22
 * @version  V0.5.0
 * 
 * @note     Initial Draft
 * @note History:
 * @note     2019/06/13 V1.0.0 mzm modify specification
 * @warning  
 */
#ifndef _OPDEVSDK_HKA_TYPES_H_
#define _OPDEVSDK_HKA_TYPES_H_

#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef unsigned int HRESULT;
#endif /* _HRESULT_DEFINED */

#define OPDEVSDK_HKA_ENUM_END              0xFFFFFFFF ///< end of enum, for alignment
#define OPDEVSDK_HKA_MEM_TAB_NUM           8          ///< max number of mem tab
#define OPDEVSDK_HKA_HANDLE_NUM            8          ///< max number of handle
#define OPDEVSDK_HKA_BLOB_MAX_DIM          8          ///< max dim of blob
#define OPDEVSDK_HKA_VERSION_MAX_CHAR      64         ///< max char number of version information

/**@brief memory alignment attributes
 */
typedef enum
{
    OPDEVSDK_HKA_MEM_ALIGN_4BYTE    = 4,
    OPDEVSDK_HKA_MEM_ALIGN_8BYTE    = 8,
    OPDEVSDK_HKA_MEM_ALIGN_16BYTE   = 16,
    OPDEVSDK_HKA_MEM_ALIGN_32BYTE   = 32,
    OPDEVSDK_HKA_MEM_ALIGN_64BYTE   = 64,
    OPDEVSDK_HKA_MEM_ALIGN_128BYTE  = 128,
    OPDEVSDK_HKA_MEM_ALIGN_256BYTE  = 256,

    OPDEVSDK_HKA_MEM_ALIGN_END      = OPDEVSDK_HKA_ENUM_END
} OPDEVSDK_HKA_MEM_ALIGNMENT_EN;


/**@brief memory complex attributes
*/
typedef enum
{
    OPDEVSDK_HKA_MEM_SCRATCH   = 1,                   ///< can be complex
    OPDEVSDK_HKA_MEM_PERSIST   = 2,                   ///< can not be complex 

    OPDEVSDK_HKA_MEM_ATTRS_END = OPDEVSDK_HKA_ENUM_END
} OPDEVSDK_HKA_MEM_ATTRS_EN;

/**@brief memory space£¬types of memory
*/
typedef enum
{
    OPDEVSDK_HKA_MEM_SPACE_CPU               = 1,     ///< normal memory type
    OPDEVSDK_HKA_MEM_SPACE_CACHED            = 2,     ///< memory for npu with cache 
    OPDEVSDK_HKA_MEM_SPACE_NOCACHED          = 3,     ///< memory for npu without cache
    OPDEVSDK_HKA_MEM_SPACE_CACHED_PRIOPITY   = 4,     ///< cached memory preferred to npu
    OPDEVSDK_HKA_MEM_SPACE_NOCACHED_PRIOPITY = 5,     ///< non-cached memory preferred to npu

    OPDEVSDK_HKA_MEM_EXTERNAL_END = OPDEVSDK_HKA_ENUM_END
} OPDEVSDK_HKA_MEM_SPACE_EN;

/**@brief rectangle(float)
 */
typedef struct
{
    float x;
    float y;
    float width;
    float height;
} OPDEVSDK_HKA_RECT_F_ST;

/**@brief memory table
*/
typedef struct
{
    size_t                         size;              ///< size of memory in unit of byte
    OPDEVSDK_HKA_MEM_ALIGNMENT_EN  alignment;         ///< memory alignment attributes, eg. 128 
    OPDEVSDK_HKA_MEM_SPACE_EN      space;             ///< types of memory
    OPDEVSDK_HKA_MEM_ATTRS_EN      attrs;             ///< memory complex attributes
    void                          *base;              ///< virtual addr of memory
    void                          *phy_base;          ///< physical addr of memory
    char                           reserved[8];
} OPDEVSDK_HKA_MEM_TAB_ST;

/**@brief memory buffer
 */
typedef struct
{
    void     *start;                                  ///< start pos
    void     *end;                                    ///< end pos
    void     *cur_pos;                                ///< current pos

} OPDEVSDK_HKA_BUF_ST;

/**@brief YUV data format
 */ 
typedef enum
{ 
    OPDEVSDK_HKA_YUV420  = 0,      ///< U/V vertical and horizontal 1/2 down sample, Y planner
    OPDEVSDK_HKA_YUV422  = 1,      ///< U/V horizontal 1/2 down sample, Y planner
    OPDEVSDK_HKA_YV12    = 2,      ///< U/V vertical and horizontal 1/2 down sample, planner, store in order Y/V/U
    OPDEVSDK_HKA_UYVY    = 3,      ///< YUV422 interleaveded
    OPDEVSDK_HKA_YUV444  = 4,      ///< U/V no down sample, planner, store in order Y/U/V
    OPDEVSDK_HKA_YVU420  = 5,      ///< contrast to OPDEVSDK_HKA_YUV420, V is stored before U
    OPDEVSDK_HKA_BGR     = 6,      ///< BGR, planner
    OPDEVSDK_HKA_BGRGPU  = 7,      ///< BGR, planner, data is in GPU MEM
    OPDEVSDK_HKA_BGR_PACKAGE = 8,  ///< BGR, package  

    OPDEVSDK_HKA_YUV_END = OPDEVSDK_HKA_ENUM_END

} OPDEVSDK_HKA_YUV_FORMAT_EN;

/**@brief DL process types
 */
typedef enum 
{
    OPDEVSDK_HKA_DL_PROC_UNKNOW  = 0,
    OPDEVSDK_HKA_DL_PROC_FP32    = 1,               ///< FP32
    OPDEVSDK_HKA_DL_PROC_FP16    = 2,               ///< FP16
    OPDEVSDK_HKA_DL_PROC_INT8    = 3,               ///< INT8
    OPDEVSDK_HKA_DL_PROC_TYPE_0  = 4,               ///<                
    OPDEVSDK_HKA_DL_PROC_TYPE_1  = 5,               ///< 
    OPDEVSDK_HKA_DL_PROC_TYPE_2  = 6,               ///< 
    OPDEVSDK_HKA_DL_PROC_TYPE_3  = 7,               ///< NPU0 for H5\H7\G3
    OPDEVSDK_HKA_DL_PROC_TYPE_4  = 8,               ///< NPU1 for H5\H7

    OPDEVSDK_HKA_DL_PROC_END     = OPDEVSDK_HKA_ENUM_END

} OPDEVSDK_HKA_DL_PROCESS_TYPE_EN;


/**@brief DL process data type
 */
typedef enum
{
    OPDEVSDK_HKA_DATA_U08 = 0x00000001,
    OPDEVSDK_HKA_DATA_S08 = 0x00000002,

    OPDEVSDK_HKA_DATA_U16 = 0x10000001,
    OPDEVSDK_HKA_DATA_S16 = 0x10000002,
    OPDEVSDK_HKA_DATA_F16 = 0x10000003,

    OPDEVSDK_HKA_DATA_U32 = 0x20000001,
    OPDEVSDK_HKA_DATA_S32 = 0x20000002,
    OPDEVSDK_HKA_DATA_F32 = 0x20000003,

    OPDEVSDK_HKA_DATA_U64 = 0x30000001,
    OPDEVSDK_HKA_DATA_S64 = 0x30000002,
    OPDEVSDK_HKA_DATA_F64 = 0x30000003,

    OPDEVSDK_HKA_DATA_END = OPDEVSDK_HKA_ENUM_END

} OPDEVSDK_HKA_DATA_TYPE_EN;

/**@brief DL process data format
 */
typedef enum
{
    OPDEVSDK_HKA_FORMAT_NCHW = 0,              ///< nchw
    OPDEVSDK_HKA_FORMAT_NHCW,                  ///< nhcw
    OPDEVSDK_HKA_FORMAT_NHWC,                  ///< nhwc
    OPDEVSDK_HKA_FORMAT_NWHC,                  ///< nwhc
    OPDEVSDK_HKA_FORMAT_NCWH,                  ///< ncwh
    OPDEVSDK_HKA_FORMAT_NWCH,                  ///< nwch
    OPDEVSDK_HKA_FORMAT_NCLHW,                 ///< nclhw
    OPDEVSDK_HKA_FORMAT_NHCLW,                 ///< nhclw
    OPDEVSDK_HKA_FORMAT_NUM = OPDEVSDK_HKA_ENUM_END
} OPDEVSDK_HKA_LAYOUT_FORMAT_EN;

/**@brief struct of blob
 */
typedef struct
{
    int                           dim;
    int                           stride[OPDEVSDK_HKA_BLOB_MAX_DIM];
    int                           shape[OPDEVSDK_HKA_BLOB_MAX_DIM];
    OPDEVSDK_HKA_LAYOUT_FORMAT_EN format;
    OPDEVSDK_HKA_DATA_TYPE_EN     type;
    OPDEVSDK_HKA_MEM_SPACE_EN     space;
    void                         *data;
    char                          reserved[8];
} OPDEVSDK_HKA_BLOB_ST;


/**@brief custom layer callbacks
 */ 
typedef struct
{
    void          *get_model_memsize;
    void          *create_model;
    void          *get_memsize;
    void          *create;
    void          *forward;
    void          *reshape;
    void          *reserved[8];                     ///< reserved cbs
} OPDEVSDK_HKA_CUSTOM_LAYER_CB_ST;

/**@brief model info
 */ 
typedef struct
{
    OPDEVSDK_HKA_DL_PROCESS_TYPE_EN       proc_type;         ///< ref to OPDEVSDK_HKA_DL_PROCESS_TYPE
    unsigned int                          model_size;        ///< model size
    void                                 *model_buf;         ///< model buffer, used to load model file
    OPDEVSDK_HKA_CUSTOM_LAYER_CB_ST       custom_cb;         ///< custom layer callbacks
    char                                  reserved[64];      ///< reserved bytes
} OPDEVSDK_HKA_MODEL_INFO_ST;

/**@brief version info
 */ 
typedef struct 
{
    char    version[OPDEVSDK_HKA_VERSION_MAX_CHAR];        ///< version information
}OPDEVSDK_HKA_VERSION_ST;

#ifdef __cplusplus
}
#endif 

#endif /* _OPDEVSDK_HKA_TYPES_H_ */

