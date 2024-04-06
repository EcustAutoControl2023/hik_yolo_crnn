
/**@file     YoloV5_s6_Region.c
 * @note     2012-2021 HangZhou Hikvision Digital Technology Co., Ltd. All Right
 * Reserved.
 * @brief    internal implementation of yolo process module in custom layer
 * standard
 *
 * @author   guixinzhe
 * @date     2021/02/04
 * @version  V01.0.0
 *
 * @note     Initial Draft
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "YoloV5_s6_Region.h"

///< #define TEST_YLOUT_PRINT (1)
///< #define TEST_YLOUT_TIME  (1)

#ifdef DEBUG_CUSTOM_LAYER
#define CHECK_ERROR(sts, str, ret)                                           \
    {                                                                        \
        if (sts)                                                             \
        {                                                                    \
            printf("File: %s, Line: %05d: %s\n", __FILE__, __LINE__, (str)); \
            return (ret);                                                    \
        }                                                                    \
    }
#define HKA_CHECK_ERROR(flag, sts) \
    {                              \
        if (flag)                  \
        {                          \
            return (sts);          \
        }                          \
    }
#define LOG_INFO printf
#else
#define CHECK_ERROR(sts, str, ret) \
    {                              \
        if (sts)                   \
        {                          \
            return (ret);          \
        }                          \
    }
#define HKA_CHECK_ERROR(flag, sts) \
    {                              \
        if (flag)                  \
        {                          \
            return (sts);          \
        }                          \
    }
#define LOG_INFO(...)
#endif
static void activate_array(float *x, const int n);

/**@fn         nms_comparator
*  @brief      create the comparator of qsort 
*  @param[in]  const void           *pa first  compare object
*              const void           *pb second compare object
*  @param[out] 
*  @return     status
*  @see
*/
static int nms_comparator(const void *pa, const void *pb)
{
    POST_PROC_DET_STACK_T a = *(POST_PROC_DET_STACK_T *)pa;
    POST_PROC_DET_STACK_T b = *(POST_PROC_DET_STACK_T *)pb;
    float diff = 0;

    ///< compare class prob if clasee is sorted
    if (b.sort_class >= 0)
    {
        diff = a.prob[b.sort_class] - b.prob[b.sort_class];
    }
    ///< compare with object prob
    else
    {
        diff = a.objectness - b.objectness;
    }
    if (diff < 0)
    {
        return 1;
    }
    else if (diff > 0)
    {
        return -1;
    }
    return 0;
}

/**@fn         box_overlap
*  @brief      compute the overlap length of two box
*  @param[in]  float                *x1 x/y coordinate of first box
*              float                *w1 width/height of first box
*              float                *x2 x/y coordinate of second box
*              float                *w2 width/height of second box
*  @param[out] 
*  @return      float
*  @see
*/
static float box_overlap(float x1,
                         float w1,
                         float x2,
                         float w2)
{
    float l1 = x1 - w1 / 2;
    float l2 = x2 - w2 / 2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1 / 2;
    float r2 = x2 + w2 / 2;
    float right = r1 < r2 ? r1 : r2;

    return right - left;
}

/**@fn         box_intersection   
*  @brief      compute intersection size of two box
*  @param[in]  BBOX_T                 a first box
*              BBOX_T                 b second box
*  @param[out]
*  @return      float
*  @see
*/
static float box_inter(BBOX_T a, BBOX_T b)
{
    float w = box_overlap(a.x, a.w, b.x, b.w); ///< x direction overlap length
    float h = box_overlap(a.y, a.h, b.y, b.h); ///< y direction overlap lenght
    if (w < 0 || h < 0)
        return 0;
    float area = w * h;

    return area;
}

/**@fn         box_union
*  @brief      compute union size of two box
*  @param[in]  BBOX_T                 a first box
*              BBOX_T                 b second box
*  @param[out]
*  @return     float
*  @see
*/
static float box_union(BBOX_T a, BBOX_T b)
{
    float i = box_inter(a, b);
    float u = a.w * a.h + b.w * b.h - i;

    return u;
}

/**@fn         box_iou
*  @brief      compute iou size of two box
*  @param[in]  BBOX_T                 a first box
*              BBOX_T                 b second box
*  @param[out]
*  @return     float                    iou
*  @see
*/
static float box_iou(BBOX_T a, BBOX_T b)
{
    return box_inter(a, b) / box_union(a, b);
}

/**@fn         entry_index 
* @brief       get data index in the memory
* @param[in]   int                          *batch        batch size
*              int                           location     grid index
*              int                           entry        information class
*              OPDEVSDK_HKA_BLOB_ST          blob_shape[] shape of blob
*              int                           anchor_num   bounding box num per grid
* @param[out]
* @return      int                                        index of object
* @see
*/
static int entry_index(int batch, int location, int entry, int blob_shape[], int anchor_num)
{
    int stride = CUSTOM_STRIDE_ALIGN(blob_shape[3]); ///< row data size in 16-bytes aligned memory
    int *shape = blob_shape;                         ///< conv out data shape
    int n = location / (shape[2] * shape[3]);
    int loc = location % (shape[2] * shape[3]);
    int h_loc = loc / shape[3];
    int w_loc = loc % shape[3];

    return batch * (shape[1] * shape[2] * stride) + n * shape[2] * stride * (shape[1] / anchor_num) + entry * shape[2] * stride + h_loc * stride + w_loc;
}

/**@fn         draw_detections_output
*  @brief      compute yolo detection output
*  @param[in]  detection                    *dets           detection result
*              int                           num            detection result num
*              float                         thresh         class prob thresh
*              int                           classes        class num
*              int                           w              wdith of image
*              int                           h              height of image
*  @param[out] void                         *output         final dection result
*              int                          *output_num     final dection num
*  @return     void
*  @see
*/
static void draw_detections_output(POST_PROC_DET_STACK_T *dets, int num, float thresh, int classes, int w, int h, void *output, int *output_num)
{
    YOLO_REGION_OUTPUT_T *output_box_info = (YOLO_REGION_OUTPUT_T *)output;

    int box_idx = 0;
    int i = 0;
    int j = 0;
    int temp_output_num = 0;

    *output_num = 0;

    for (i = 0; i < num; ++i)
    {
        int cla = -1;
        float old_best_score = 0;
        for (j = 0; j < classes; j++)
        {

            if (dets[i].prob[j] >= thresh)
            {
                if (dets[i].prob[j] >= old_best_score)
                {
                    old_best_score = dets[i].prob[j];
                    if (cla < 0)
                    {
                        cla = j;
                    }
                    output_box_info[box_idx].score = dets[i].prob[j];
                    output_box_info[box_idx].class_type = (float)j;
                    output_box_info[box_idx].batch_idx = dets[i].batch_index;
                }
            }
        }

        if (cla >= 0)
        {
            ///< compute relative coordinates of box in test image size
            BBOX_T b = dets[i].bbox;
            int left = (b.x - b.w / 2.);  ///< im.w;
            int right = (b.x + b.w / 2.); ///< im.w;
            int top = (b.y - b.h / 2.);   ///< im.h;
            int bot = (b.y + b.h / 2.);   ///< im.h;

            if (left < 0)
            {
                left = 0;
            }
            if (right > w)
            {
                right = w;
            }
            if (top < 0)
            {
                top = 0;
            }
            if (bot > h)
            {
                bot = h;
            }
            output_box_info[box_idx].bbox.x = left;
            output_box_info[box_idx].bbox.y = top;
            output_box_info[box_idx].bbox.w = right - left;
            output_box_info[box_idx].bbox.h = bot - top;
            box_idx++;
            temp_output_num++;
        }
    }

    *output_num = temp_output_num;
#if TEST_YLOUT_PRINT
    FILE *fp = NULL;
    fp = fopen("box.txt", "wb");
    for (i = 0; i < temp_output_num; i++)
    {
        fprintf(fp, "%f %f %f %f %f\n", output_box_info[i].score, output_box_info[i].bbox.x,
                output_box_info[i].bbox.y, output_box_info[i].bbox.w,
                output_box_info[i].bbox.h);
    }

    fclose(fp);
#endif
}
#if 0
/**@fn          count_before_nms
*  @brief       remove detection if onject prob is 0
*  @param[in]   POST_PROC_DET_STACK_T        *dets     dection result  
*               int                           total    detection result number
*  @param[out]  detection                    *dets     dection result
*  @return      int
*  @see
*/
static int count_before_nms(POST_PROC_DET_STACK_T *dets, int total)
{
    int i, k;
    k = total - 1;

    for (i = 0; i <= k; ++i)
    {
        if (dets[i].objectness == 0)
        {
            POST_PROC_DET_STACK_T swap = dets[i];
            dets[i]        = dets[k];
            dets[k]        = swap;
            --k;
            --i;
        }
    }
    total = k + 1;
#if TEST_YLOUT_PRINT
    FILE *fp1 = NULL;
    fp1       = fopen("countxxxx.txt", "wb");
    for (i = 0; i < total; i++)
    {
        fprintf(fp1, "%f %f %f %f %f %f\n", dets[i].prob[0], dets[i].prob[1], dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h);
    }

    fclose(fp1);
#endif
    return total;
}
#endif
/**@fn           do_nms_sort
*   @brief       non naximum suppression
*   @param[in]   POST_PROC_DET_STACK_T        *dets      dection result  
*                int                           total     dection result  number
*                int                           classes   class num
*                float                         thresh    nms thresh
*   @param[out]  POST_PROC_DET_STACK_T        *dets      dection result  
*   @return      void
*   @see
*/
static void do_nms_sort(POST_PROC_DET_STACK_T *dets, int total, int classes, float thresh)
{
    int i, j, k;
#if TEST_YLOUT_PRINT
    FILE *fp_1 = NULL;
    fp_1 = fopen("sort.txt", "wb");
    for (i = 0; i < total; i++)
    {
        fprintf(fp_1, "%f %f %f %f %f %f\n", dets[i].prob[0], dets[i].prob[5], dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h);
    }

    fclose(fp_1);
#endif

    for (k = 0; k < classes; ++k)
    {
        ///< init sort class
        for (i = 0; i < total; ++i)
        {
            dets[i].sort_class = k;
        }
        ///< sort detection with nms_comparator
        qsort(dets, total, sizeof(POST_PROC_DET_STACK_T), nms_comparator);
        for (i = 0; i < total; ++i)
        {
            if (dets[i].prob[k] == 0)
            {
                continue;
            }
            BBOX_T a = dets[i].bbox;
            for (j = i + 1; j < total; ++j)
            {
                BBOX_T b = dets[j].bbox;
                if (box_iou(a, b) > thresh)
                {
                    dets[j].prob[k] = 0;
                }
            }
        }
    }
#if TEST_YLOUT_PRINT
    FILE *fp = NULL;
    fp = fopen("nms.txt", "wb");
    for (i = 0; i < total; i++)
    {
        fprintf(fp, "%f %f %f %f %f %f\n", dets[i].prob[0], dets[i].prob[5], dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h);
    }

    fclose(fp);
#endif
}

/**@fn         get_region_box
*  @brief      transfor predications data to box info
*  @param[in]  float                        *x       predications
*              float                        *biases  anchors
*              int                           n       anchors index
*              int                           index   box start index in memory
*              int                           i       row index of current box
*              int                           j       column index of current box
*              int                           w       conv size
*              int                           h       conv size
*              int                           stride  stride between different info of the same box in memory
*              int                           input_w imgage width
*              int                           input_h imgage height
*  @return     BBOX_T                                box to be computed
*  @see
*/
static BBOX_T get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h, int stride, int input_w, int input_h)
{
    BBOX_T b;
    int k = 0;
    int conv1_size = 0;
    conv1_size = input_w / UPSAMPLE_RATIO;
    int st = input_w / w;
    switch (w / conv1_size)
    {
    case (1):
        k = n;
        break;
    case (2):
        k = n + 3;
        break;
    case (4):
        k = n + 6;
        break;
    case (8):
        k = n + 9;
    default:
        break;
    }

    b.x = (i - 0.5 + x[index + 0 * stride] * 2) * st;
    b.y = (j - 0.5 + x[index + 1 * stride] * 2) * st;
    b.w = x[index + 2 * stride] * x[index + 2 * stride] * 4 * biases[2 * k];
    b.h = x[index + 3 * stride] * x[index + 3 * stride] * 4 * biases[2 * k + 1];

    ///< printf("%f %f %f %f %f %f %f %f\n", x[index + 0*stride], x[index + 1*stride],x[index + 2*stride],x[index + 3*stride], b.x, b.y, b.w, b.h);
    ///< printf("%d %d\n", biases[2*k], biases[2*k+1]);
    ///< printf("%d %d\n", st, stride);
    ///< printf("%d %d\n", i, j);
    return b;
}
#if 0
/**@fn          correct_region_boxes
*  @brief       correct box info with orignal image
*  @param[in]   POST_PROC_DET_STACK_T        *dets    dection result 
*               int                           n       dection result number
*               int                           w       test image width
*               int                           h       test image height
*               int                           netw    original test image width
*               int                           neth    original test image height
*  @param[out]  POST_PROC_DET_STACK_T        *dets    dection result 
*  @return      void
*  @see
*/ 
static void correct_region_boxes(POST_PROC_DET_STACK_T *dets, int n, int w, int h, int netw, int neth)
{
    int i;
    int new_w = 0;
    int new_h = 0;

    if (((float)netw / w) < ((float)neth / h)) 
    {
        new_w = netw;
        new_h = (h * netw) / w;
    }
    else 
    {
        new_h = neth;
        new_w = (w * neth)/h;
    }
    for (i = 0; i < n; ++i)
    {
        BBOX_T b = dets[i].bbox;
        b.x =  (b.x - (netw - new_w)/2./netw) / ((float)new_w/netw); 
        b.y =  (b.y - (neth - new_h)/2./neth) / ((float)new_h/neth); 
        b.w *= (float)netw/new_w;
        b.h *= (float)neth/new_h;
        dets[i].bbox = b;
    }
}
#endif
/**@fn          get_region_detections
*  @brief       get detection result from conv presdications data
*  @param[in]   int                      w                  test image width
*               int                      h                  test image height
*               float                    thresh             class prob thresh
*               int                     *blob_data          conv out data blob
*               int                      blob_shape[]       conv out data blob shape
*               int                     *anchor_num         anchor num
*               int                     *class_num          class num
*  @param[out]  POST_PROC_DET_STACK_T   *dets               dection result
*  @return      void
*  @see
*/
int get_region_detections(int w,
                          int h,
                          float thresh,
                          float *blob_data,
                          int blob_shape[],
                          int anchor_num,
                          int class_num,
                          POST_PROC_DET_STACK_T *dets, int *real_bbx_num)
{
    int i = 0;
    int n = 0;
    int j = 0;
    float anchors[] = {436, 615, 739, 380, 925, 792, 140, 301, 303, 264, 238, 542, 96, 68, 86, 152, 180, 137, 19, 27, 44, 40, 38, 94}; ///< anchors
    float *predictions = blob_data;                                                                                                    ///< conv out data
    int *shape = blob_shape;                                                                                                           ///< conv out data shape
    int stride = CUSTOM_STRIDE_ALIGN(blob_shape[3]);

    // POST_PROC_DET_STACK_T stdetOut[128] = {0};
    int detOutNum = 0;
    // memset(stdetOut, 0, sizeof(POST_PROC_DET_STACK_T) * 128);
    for (int batch_index = 0; batch_index < shape[0]; batch_index++)
    {
        for (i = 0; i < shape[2] * shape[3]; ++i)
        {
            int row = i / shape[3];
            int col = i % shape[3];
            for (n = 0; n < anchor_num; ++n)
            {
                int index = n * shape[2] * shape[3] + i;

                // objectness
                int obj_index = entry_index(batch_index, index, 4, blob_shape, anchor_num);
                float objectness = predictions[obj_index];
                dets[detOutNum].objectness = (objectness > thresh) ? objectness : 0;

                if (dets[detOutNum].objectness == 0)
                {
                    continue;
                }

                // box
                int box_index = entry_index(batch_index, index, 0, shape, anchor_num);
                dets[detOutNum].bbox = get_region_box(predictions, anchors, n, box_index, col, row, shape[3], shape[2], shape[2] * stride, w, h);

                // classes
                dets[detOutNum].classes = class_num;
                dets[detOutNum].batch_index = batch_index;
                // score = objectness * class_prob
                int class_flag = 0;
                for (j = 0; j < class_num; j++)
                {
                    dets[detOutNum].prob[j] = 0;
                }
                for (j = 0; j < class_num; j++)
                {
                    int class_index = entry_index(batch_index, index, 4 + 1 + j, shape, anchor_num);
                    float prob = objectness * predictions[class_index];
                    ///< dets[index].prob[0] = prob;
                    if (prob > thresh)
                    {
                        dets[detOutNum].prob[j] = prob;
                        class_flag = 1;
                    }
                    else
                    {
                        dets[detOutNum].prob[j] = 0;
                    }
                }
                if (class_flag)
                {
                    detOutNum++;
                }
            }
        }
    }
    return detOutNum;
}

/**@fn          logistic_activate
*  @brief       compute logistic value
*  @param[in]   float    x      input data
*  @return      float           activated data
*  @see
*/
static inline float logistic_activate(float x)
{
    return 1. / (1. + exp(-x));
}

/**@fn           activate_array
 *  @brief       call logistic activate func
 *  @param[in]   float          *x input data
 *               const int       n input num
 *  @param[out]  float          *x both input and output data
 *  @return      void
 *  @see
 */
static void activate_array(float *x, const int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        x[i] = logistic_activate(x[i]);
    }
}
#if 0
/**@fn          forward_region_layer
*  @brief       forward func: acticate the predications value
*  @param[in]   float     *blob_data     input value
*               int        blob_shape[]  shape of blob
*               int        anchor_num    anchors num
*               int        class_num     class num
*  @param[out]
*  @return      float*
*  @see
*/
static float *forward_region_layer(float *blob_data, int blob_shape[], int anchor_num, int class_num)
{
#if TEST_YLOUT_PRINT
    int i     = 0;
#endif
    int b     = 0;
    int n     = 0;
    int index = 0;

    int *  shape        = blob_shape;
    int    stride       = CUSTOM_STRIDE_ALIGN(blob_shape[3]);
    float *l_output_raw = blob_data;

#if TEST_YLOUT_PRINT
    FILE *result_fp = NULL;
    FILE *fp        = NULL;
    fp              = fopen("output.txt", "wb");
    printf("forward_region_layer shape:%d %d %d %d stride:%d total:%d\n",
           blob_shape[0], blob_shape[1], blob_shape[2], blob_shape[3], stride,
           shape[1] * shape[2] * stride * shape[0]);
    for (i = 0; i < (shape[1] * shape[2] * stride * shape[0]); i++)
    {
        fprintf(fp, "%f\n", l_output_raw[i]);
    }
#endif
    /*printf("forward_region_layer shape:%d %d %d %d stride:%d total:%d\n",
           blob_shape[0], blob_shape[1], blob_shape[2], blob_shape[3], stride,
           shape[1] * shape[2] * stride * shape[0]);
	printf("%s %d shape [%d %d %d %d] anchor_num %d\n",__FUNCTION__,__LINE__,shape[0],shape[1],shape[2],shape[3],anchor_num);*/

    for (b = 0; b < shape[0]; b++)
    {
        for (n = 0; n < anchor_num; n++)
        {
            ///< activate x,y info
            index = entry_index(b, n * shape[2] * shape[3], 0, blob_shape, anchor_num);
            ///< yolov5 activate num is 4*shape[2]*stride
            //activate_array(l_output_raw + index, 4 * shape[2] * stride);

            ///< activate class, object info
            index = entry_index(b, n * shape[2] * shape[3], 4, blob_shape, anchor_num);
            //activate_array(l_output_raw + index, (1 + class_num) * shape[2] * stride);
        }
    }

#if TEST_YLOUT_PRINT
    printf("forward_region_layer shape:%d %d %d %d stride:%d total:%d\n",
           blob_shape[0], blob_shape[1], blob_shape[2], blob_shape[3], stride,
           shape[1] * shape[2] * stride * shape[0]);
    result_fp = fopen("output finish.txt", "wb");
    for (i = 0; i < (shape[1] * shape[2] * stride * shape[0]); i++)
    {
        fprintf(result_fp, "%f\n", l_output_raw[i]);
    }
    fclose(result_fp);
    fclose(fp);
#endif

    return l_output_raw;
}
#endif
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
                             int *assist_mem_size, int outshape[4])
{
    outshape[0] = 1;
    outshape[1] = 1;
    outshape[2] = in1_n * in1_h * in1_w * num_anchors + in2_n * in2_h * in2_w * num_anchors + in3_n * in3_h * in3_w * num_anchors + in4_n * in4_h * in4_w * num_anchors;
    outshape[3] = POST_PROC_DET_OUT_DIM;
    *assist_mem_size = sizeof(POST_PROC_DET_STACK_T) * (in1_h * in1_w + in2_h * in2_w + in3_h * in3_w + in4_h * in4_w) * YOLOV5_NMS_BLOB_NUM;

    return 0;
}

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
                             float *output, int *outshape,
                             int num_anchors, int num_classes, float conf_thresh, float nms_thresh,
                             char *__auxi_mem_addr__)
{
#if TEST_YLOUT_TIME
    long long L1 = 0;
    long long L2 = 0;
    struct timeval tv;
    long long a_gemm = 0.0;
    long long b_gemm = 0.0;
    long long c_gemm = 0.0;
    long long d_gemm = 0.0;
    long long e_gemm = 0.0;
    long long f_gemm = 0.0;
    long long g_gemm = 0.0;
#endif

    //int sts  = -1;
#if TEST_YLOUT_PRINT
    int i = 0;
#endif

    POST_PROC_DET_STACK_T *detections_data = NULL;

    int height = 0;
    int width = 0;
    int output_num = 0;

    int anchor_num = num_anchors;
    int class_num = num_classes;

    //int batch_size     = 0;
    int feat_height = 0;
    int feat_width = 0;
    int feat_size_real = 0;
    //int stride         = 0;

    ///< loop variable
    int raw_bbx_num = 0;
    int real_bbx_num = 0;
    int total_raw_bbx_num = 0;
    int total_bbx_num = 0;
    int blob_count = 0;

    const int blob_num = YOLOV5_NMS_BLOB_NUM;

    float *blob_data[YOLOV5_NMS_BLOB_NUM] = {0};
    int blob_shape[YOLOV5_NMS_BLOB_NUM][4] = {0};

#if TEST_YLOUT_PRINT
    printf("num_anchors: %d num_classes: %d\n", num_anchors, num_classes);
    printf("conf_thresh: %f nms_thresh: %f\n", conf_thresh, nms_thresh);
#endif

    ///< get yolo layer input size
    width = UPSAMPLE_RATIO * in4_w;
    height = UPSAMPLE_RATIO * in4_h;
    // CHECK_ERROR(width != height, "yolo layer feature map isn equal!!!\n", -1);

    ///< gather detection result of all the conv output feature map in one piece of memory
    blob_data[0] = y1;
    blob_shape[0][0] = in1_n;
    blob_shape[0][1] = in1_c;
    blob_shape[0][2] = in1_h;
    blob_shape[0][3] = in1_w;

    blob_data[1] = y2;
    blob_shape[1][0] = in2_n;
    blob_shape[1][1] = in2_c;
    blob_shape[1][2] = in2_h;
    blob_shape[1][3] = in2_w;

    blob_data[2] = y3;
    blob_shape[2][0] = in3_n;
    blob_shape[2][1] = in3_c;
    blob_shape[2][2] = in3_h;
    blob_shape[2][3] = in3_w;

    blob_data[3] = y4;
    blob_shape[3][0] = in4_n;
    blob_shape[3][1] = in4_c;
    blob_shape[3][2] = in4_h;
    blob_shape[3][3] = in4_w;

    detections_data = (POST_PROC_DET_STACK_T *)__auxi_mem_addr__;
    for (blob_count = 0; blob_count < blob_num; blob_count++)
    {
        //batch_size     = blob_shape[blob_count][0];
        feat_height = blob_shape[blob_count][2];
        feat_width = blob_shape[blob_count][3];
        feat_size_real = feat_height * feat_width;

#if TEST_YLOUT_PRINT
        printf("batch:%d\n", blob_shape[blob_count][0]);
        printf("channel:%d\n", blob_shape[blob_count][1]);
        printf("height: %d\n", blob_shape[blob_count][2]);
        printf("width: %d\n", blob_shape[blob_count][3]);
#endif

        raw_bbx_num = feat_size_real * anchor_num; ///< total bbx num

        // #if TEST_YLOUT_TIME
        //         gettimeofday(&tv, NULL);
        //         L1 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
        // #endif
        //放到 get_region_detections函数，选择性激活
        //forward_region_layer(blob_data[blob_count], blob_shape[blob_count], anchor_num, class_num); ///< compute forward activation value

        // #if TEST_YLOUT_TIME
        //         gettimeofday(&tv, NULL);
        //         L2     = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
        //         a_gemm = L2 - L1;
        //         printf("forward_region_layer :%lld us\n", a_gemm);
        // #endif

#if TEST_YLOUT_TIME
        gettimeofday(&tv, NULL);
        L1 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
#endif

        ///< get all detection result
        real_bbx_num = get_region_detections(width, height, conf_thresh, blob_data[blob_count], blob_shape[blob_count], anchor_num, class_num, detections_data + total_raw_bbx_num, &real_bbx_num);

#if TEST_YLOUT_TIME
        gettimeofday(&tv, NULL);
        L2 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
        b_gemm = L2 - L1;
        printf("get_region_detections :%lld us\n", b_gemm);
#endif

#if TEST_YLOUT_TIME
        gettimeofday(&tv, NULL);
        L1 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
#endif
        ///< remove detection result if objectness is 0
        //放到 get_region_detections函数，有效数据添加
        //real_bbx_num = count_before_nms(detections_data + total_raw_bbx_num, raw_bbx_num);

        ///< move current detection result to memory with controlling the offset
        memcpy((POST_PROC_DET_STACK_T *)detections_data + total_bbx_num, (POST_PROC_DET_STACK_T *)detections_data + total_raw_bbx_num, real_bbx_num * sizeof(POST_PROC_DET_STACK_T));

#if TEST_YLOUT_TIME
        gettimeofday(&tv, NULL);
        L2 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
        c_gemm = L2 - L1;
        printf("count_before_nms :%lld us\n", c_gemm);
#endif

#if TEST_YLOUT_PRINT
        FILE *fp_m = NULL;
        fp_m = fopen("mem.txt", "wb");
        for (i = 0; i < real_bbx_num; i++)
        {
            fprintf(fp_m, "%f %f %f %f %f\n", detections_data[total_bbx_num + i].prob[0], detections_data[total_bbx_num + i].bbox.x, detections_data[total_bbx_num + i].bbox.y, detections_data[total_bbx_num + i].bbox.w, detections_data[total_bbx_num + i].bbox.h);
        }
        fclose(fp_m);
#endif
        ///< sum up all the bounding box
        total_raw_bbx_num += raw_bbx_num;
        total_bbx_num += real_bbx_num;
        ///< printf("%d-%d-%d-%d\n", raw_bbx_num, real_bbx_num, total_raw_bbx_num, total_bbx_num);
    }

#if TEST_YLOUT_PRINT
    FILE *fp_o = NULL;
    fp_o = fopen("after.txt", "wb");
    for (i = 0; i < total_bbx_num; i++)
    {
        fprintf(fp_o, "%f %f %f %f %f\n", detections_data[i].prob[0], detections_data[i].bbox.x, detections_data[i].bbox.y, detections_data[i].bbox.w, detections_data[i].bbox.h);
    }

    fclose(fp_o);
#endif

#if TEST_YLOUT_TIME
    gettimeofday(&tv, NULL);
    L1 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
#endif

    ///< nms sort for all the detection
    if (nms_thresh)
    {
        do_nms_sort((POST_PROC_DET_STACK_T *)detections_data, total_bbx_num, class_num, nms_thresh);
    }

    ///< get final dectection result relative to the test image size
    draw_detections_output((POST_PROC_DET_STACK_T *)detections_data, total_bbx_num, conf_thresh, class_num, width, height, output, &output_num);

#if TEST_YLOUT_PRINT
    YOLO_REGION_OUTPUT_T *test = (YOLO_REGION_OUTPUT_T *)output;
    printf("class_num:%d total_bbx_num:%d output_num:%d\n", class_num,
           total_bbx_num, output_num);
    for (int i = 0; i < output_num; i++)
    {
        printf("class:%f score:%f rect:[%f %f %f %f]\n", test[i].class_type,
               test[i].score, test[i].bbox.x, test[i].bbox.y, test[i].bbox.w,
               test[i].bbox.h);
    }
#endif

#if TEST_YLOUT_TIME
    gettimeofday(&tv, NULL);
    L2 = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
    d_gemm = L2 - L1;
    printf("do_nms_sort+draw_detections_output :%lld us\n", d_gemm);
#endif

    outshape[0] = output_num;            ///< final output detection box num
    outshape[1] = POST_PROC_DET_OUT_DIM; ///< final output box param class1-score1-box4-batch_idx1
    outshape[2] = 1;
    outshape[3] = 1;

    return 0;
}
