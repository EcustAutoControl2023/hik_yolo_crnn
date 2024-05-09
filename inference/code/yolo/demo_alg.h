/***************************************************************************
* note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         demo_alg.h
* @brief        encapsulate four interfaces with the hikflow library 
*
* @author       guanjiajun
*
* @date         2019/2/18
* @version      1.0.0
* @note         add interfaces for hikflow
*
* @date         2019/6/26
* @version      2.0.0
* @note         add english notes
*****************************************************************************/


#ifndef _DEMO_ALG_H_
#define _DEMO_ALG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 

#include "opdevsdk_sys.h"
#include "opdevsdk_jpegenc.h"
#include "opdevsdk_pos.h"
#include "opdevsdk_hikflow_lib.h"
#include "demo_comm.h"


/**
* @brief maxism of path length
*/
#define DEMO_ALG_MAX_PATH_LENGTH                    (256)
#define MAX_OUTPUT_BOX_NUM  (64)   ///< max bounding box num for each blob
#define HIK_MODEL_NUM (2)
#define HIK_MODEL_KEY "hikflow_model_path=%s\n"

/**
* @brief CONFIG_DATA_T struct
*/
typedef struct _CONFIG_DATA_T
{
    int                             core_proc_type;              ///< processor type (8 is HIKFLOW_DL_PROC_TYPE_4 7 is HIKFLOW_DL_PROC_TYPE_3)
    int                             test_data_type;              ///< image type,0 means RGB,1 means NV21
    int                             data_mem_type;               ///< memory allocated type,0 show arm memory,1 means that memory is allocated from mem moduler in medDrv library      
    char                            hikflow_model_path[DEMO_ALG_MAX_PATH_LENGTH];     ///< model path
    char                            test_image_list[DEMO_ALG_MAX_PATH_LENGTH];        ///< image list
    char                            result_path[DEMO_ALG_MAX_PATH_LENGTH];            ///< result path
     ///< input image: n c h w
    int                             n;                           ///< the number of data can be input in one time
    int                             c;                           ///< channel num,such as the channel num of yuv or rgb is 3
    int                             h;                           ///< image width
    int                             w;                           ///< image height
    int                             flag_config_all_param;       ///< for check config has all praram
} CONFIG_DATA_T;

/**
* @brief bounding box struct
*/
typedef struct{
    float x; 
    float y;
    float w;
    float h;
} box;

/**
* @brief bounding box struct
*/
typedef struct _BBOX_T
{
    float x;
    float y;
    float w;
    float h;
}BBOX_T;

///< output struct, need 4-byte alignment
typedef struct _OUTPUT_BOX_INFO_T 
{
    float   class_type;
    float   score;
    BBOX_T  bbox;
    float   batch_idx;
} OUTPUT_BOX_INFO;

/** 
* @brief        demo_alg_init
* @param[in]    char *config_file_path 
* 
* @return       status
* @see
*/
int demo_alg_init(char *config_file_path);

/** 
* @brief        demo_alg_proc_fromCamera
* @param[in]    OPDEVSDK_VIDEO_FRAME_INFO_ST     *pfrm
                OPDEVSDK_POS_TARGET_LIST_INFO_ST *ptarget
* 
* @return       status
* @see
*/
int demo_alg_proc_fromCamera(OPDEVSDK_VIDEO_FRAME_INFO_ST *pfrm,OPDEVSDK_POS_TARGET_LIST_INFO_ST *ptarget, int check_weight_limit);

/** 
* @brief        demo_alg_proc_fromFile
* @param[in]    void
* @return       status
* @see
*/
int demo_alg_proc_fromFile();

/** 
* @brief        demo_alg_deinit
* @param[in]    void
* @return       status
* @see
*/
int demo_alg_deinit();

/** 
* @brief        demo_alg_releaseBuffer
* @param[in]    void
* @return       status
* @see
*/
int demo_alg_releaseBuffer();

int demo_alg_get_res(int *alg_w, int *alg_h);

int releaseIfErr();
#ifdef __cplusplus
}
#endif

#endif

