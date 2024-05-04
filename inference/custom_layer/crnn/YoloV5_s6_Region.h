#ifndef  _YoloV5_s6_Region_H_
#define  _YoloV5_s6_Region_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <math.h>

///< Macro Defination
// TODO: 记得改类别数
#define MAX_CLASS_NUM                 (1)          ///< max class num
#define UPSAMPLE_RATIO                (64)
#define YOLOV5_NMS_BLOB_NUM           (4)
#define POST_PROC_DET_OUT_DIM         (7)           ///< output data dim: class1-score1-box4-batch_idx1
#define POST_PROC_DET_OUT_COORDI      (4)           ///< coordinate num(no use)
#define POST_PROC_DET_OUT_MAX_NUM     (1000)        ///< max output target num(no use)
#define MAX_OUTPUT_BOX_NUM            (64)          ///< max bounding box num for each blob(no use)

#define POST_PROC_DET_MAX(a, b)             (((a) < (b)) ? (b) : (a))
#define POST_PROC_DET_MIN(a, b)             (((a) > (b)) ? (b) : (a))

#define POST_PROC_DET_ALIGN(size, alignment) (((size) + (alignment) - 1) & (~((alignment) - 1)))

///< !!!! python code should be src_w, nothing to do with stride
///< on platform h5 or h7 with npu0, should define macro NPU0CODE to align the stride 
#ifdef NPU0CODE
#define CUSTOM_STRIDE_ALIGN(size) (((size) + ((4) - 1)) & (~((4) - 1)))
#else
#define CUSTOM_STRIDE_ALIGN(size) (size)
#endif

///< bounding box struct
typedef struct _BBOX_T
{
    float x;
    float y;
    float w;
    float h;
}BBOX_T;

///< output struct, need 4-byte alignment
#pragma pack(push)
#pragma pack(4)
typedef struct _YOLO_REGION_OUTPUT_T 
{
    float   class_type;
    float   score;
    BBOX_T  bbox;
    float   batch_idx;
} YOLO_REGION_OUTPUT_T;
#pragma pack(pop)

///< dection struct
typedef struct POST_PROC_DET_STACK
{
    BBOX_T  bbox;
    int     classes;
    float   prob[MAX_CLASS_NUM];
    float   objectness;
    int     sort_class;
    int     batch_index;
} POST_PROC_DET_STACK_T;

/**@fn          YoloRegion_Reshape
*  @brief       reshape function declaration
*  @param[in]   int        in1_n,in1_c,in1_h,in1_w      anchor1 input shape 1*1*20*20
*               int        in2_n,in2_c,in2_h,in2_w      anchor2 input shape 1*1*40*40
*               int        in3_n,in3_c,in3_h,in3_w      anchor3 input shape 1*1*80*80
*               int        num_anchors                  hyperparameter
*               int       *assist_mem_size              assist memory size
*  @param[out]  int        outshape[4]                  output shape of the blob
*  @return      int
*  @see
*/
int YoloV5_s6_Region_Reshape(int in1_n, int in1_c, int in1_h, int in1_w,
                       int in2_n, int in2_c, int in2_h, int in2_w,
                       int in3_n, int in3_c, int in3_h, int in3_w,
                       int in4_n, int in4_c, int in4_h, int in4_w,
                       int num_anchors,
                       int *assist_mem_size, int outshape[4]);

/**@fn          YoloRegion_Forward
*  @brief       forward function declaration
*  @param[in]   float     *y1, *y2, *y3                     input data pointer
*               int        in1_n, in1_c, in1_h, in1_w       anchor1 input shape 1*1*20*20
*               int        in2_n, in2_c, in2_h, in2_w       anchor2 input shape 1*1*40*40
*               int        in3_n, in3_c, in3_h, in3_w       anchor3 input shape 1*1*80*80
*               int        num_anchors, num_classes         hyperparameter
*               float      conf_thresh, nms_thresh          hyperparameter
*               char      *__auxi_mem_addr__                assist memory data pointer
*  @param[out]  float     *output                           data pointer of output blob
*               int        outshape                         shape of output blob
*  @return      int
*  @see
*/
int YoloV5_s6_Region_Forward(float *y1, int in1_n, int in1_c, int in1_h, int in1_w,
                       float *y2, int in2_n, int in2_c, int in2_h, int in2_w,
                       float *y3, int in3_n, int in3_c, int in3_h, int in3_w,
                       float *y4, int in4_n, int in4_c, int in4_h, int in4_w,
                       float *output, int *outshape, int num_anchors,
                       int num_classes, float conf_thresh, float nms_thresh,
                       char *__auxi_mem_addr__);

#ifdef __cplusplus
}
#endif

#endif
