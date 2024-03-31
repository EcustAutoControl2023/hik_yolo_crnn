/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_img.h
* @brief        基础服务库图像处理模块
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加图像缩放和内存拷贝接口 
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口
*
* @date         2020/2/19
* @version      2.0.1
* @note         增加颜色填充，绘制区域、矩形等接口
*               
*****************************************************************************/

#ifndef _OPDEVSDK_IMG_H_
#define _OPDEVSDK_IMG_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_video.h"


#define OPDEVSDK_IMG_MAX_WIDTH                  (4096)      /*!< 图像处理模块支持最大处理图像宽度 */
#define OPDEVSDK_IMG_MAX_HEIGHT                 (4096)      /*!< 图像处理模块支持最大处理图像高度 */
#define OPDEVSDK_IMG_MAX_TRACE_POINT_NUM        (128)       /*!< 图像处理模块支持最大轨迹坐标点数量 */


/**
* @brief 图像缩放能力,按位使能,例如0x3表示同时支持硬件缩放和软件缩放
*/
typedef enum _OPDEVSDK_IMG_SCALE_ABILI_EN_
{
    OPDEVSDK_IMG_SCALE_UNSUPPORT        = 0,        /*!< 不支持缩放,无法使用图像缩放相关接口 */
    OPDEVSDK_IMG_SCALE_HARDWARE         = 1,        /*!< 支持硬件缩放,可使用硬件缩放接口opdevsdk_img_scale */
    OPDEVSDK_IMG_SCALE_SOFT             = 2,        /*!< 支持软件缩放,可使用软件缩放接口opdevsdk_img_softScale */
    OPDEVSDK_IMG_SCALE_MAX_NUM          = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_IMG_SCALE_ABILI_EN;


/**
* @brief 坐标点描述
*/
typedef struct _OPDEVSDK_IMG_POINT_ST_
{
    int       x;      /*!< X轴坐标,单位:像素,2像素对齐 */
    int       y;      /*!< Y轴坐标,单位:像素,2像素对齐 */
}OPDEVSDK_IMG_POINT_ST;


/**
* @brief 矩形框描述
*/
typedef struct _OPDEVSDK_IMG_RECT_PARAM_ST_
{
    OPDEVSDK_IMG_POINT_ST       point;          /*!< 矩形框左上角坐标点信息 */
    unsigned int                width;          /*!< 矩形框宽度,单位:像素,4像素对齐 */
    unsigned int                height;         /*!< 矩形框高度,单位:像素,2像素对齐 */
}OPDEVSDK_IMG_RECT_PARAM_ST;


/**
* @brief 缩放参数描述
*/
typedef struct _OPDEVSDK_IMG_SCALE_PARAM_ST_
{
    OPDEVSDK_IMG_RECT_PARAM_ST      rectParam;      /*!< 选择区域的描述,针对输入图像 */ 
    int                             res[4];         /*!< 预留字段 */
}OPDEVSDK_IMG_SCALE_PARAM_ST;


/**
* @brief 轨迹坐标点集合描述
*/
typedef struct _OPDEVSDK_IMG_TRACE_INFO_ST_
{
    unsigned int                pointNum;       /*!< 轨迹坐标点数量 */ 
    OPDEVSDK_IMG_POINT_ST       *pPoint;        /*!< 每个轨迹坐标点描述，指针类型;
                                                 *若pointNum = N，则指针pPoint指向的地址数据为N*sizeof(OPDEVSDK_IMG_POINT_ST)*/
}OPDEVSDK_IMG_TRACE_INFO_ST;


/** 
* @brief            数据拷贝接口,使用dma硬件加速
* @note             注意输入和输出都是物理地址，以此区别于memcpy
*
* @param[in]        pPhySrc         源图像数据物理地址
* @param[in]        pPhyDst         目的图像数据物理地址
* @param[in]        size            拷贝数据长度
* @param[out]       无                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaCopy1d(void *pPhySrc,void *pPhyDst,unsigned int size);


/** 
* @brief            图像数据拷贝接口,使用dma硬件加速
* @note             图像属于二维，区别于一维数据
*
* @param[in]        pSrc            源图像描述，指针类型
* @param[in]        pDst            目的图像描述，指针类型
* @param[in]        param           对源图像裁剪区域选择描述
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaCopy2d(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_RECT_PARAM_ST param);


/** 
* @brief            软件缩放接口
* @note             ARM版本缩放，区别于硬件缩放
*
* @param[in]        pSrc            源图像描述，指针类型
* @param[in]        pDst            目的图像描述，指针类型
* @param[in]        param           缩放参数
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_softScale(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_SCALE_PARAM_ST param);


/** 
* @brief            硬件缩放接口
*
* @param[in]        pSrc            源图像描述，指针类型
* @param[in]        pDst            目的图像描述，指针类型
* @param[in]        param           缩放参数
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_scale(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_SCALE_PARAM_ST param);


/** 
* @brief            图像填充色块
* @note             仅支持YUV图像
*
* @param[in]        pFrame          待处理图像描述，指针类型
* @param[in]        param           区域参数，矩形框
* @param[in]        color           待填充颜色,参考定义OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaFill(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_RECT_PARAM_ST param,unsigned int color);


/** 
* @brief            图像绘制线条
* @note             仅支持YUV图像,线条起点、终点坐标2像素对齐,水平线条要求宽度4像素对齐
*
* @param[in]        pFrame          待处理图像描述，指针类型
* @param[in]        src             线条起点坐标
* @param[in]        dst             线条终点坐标
* @param[in]        color           线条颜色,参考定义OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       线条宽度,4像素对齐
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawLine(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_POINT_ST src,OPDEVSDK_IMG_POINT_ST dst,unsigned int color,int lineWidth);


/** 
* @brief            图像绘制矩形框
* @note             仅支持YUV图像
*
* @param[in]        pFrame          待处理图像描述，指针类型
* @param[in]        param           待绘制区域参数
* @param[in]        color           矩形框颜色,参考定义OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       矩形框的线条宽度,4像素对齐
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawRect(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_RECT_PARAM_ST param,unsigned int color,int lineWidth);


/** 
* @brief            图像绘制轨迹
* @note             仅支持YUV图像
*
* @param[in]        pFrame          待处理图像描述，指针类型
* @param[in]        pTraceInfo      轨迹描述，指针类型
* @param[in]        color           轨迹颜色,参考定义OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       轨迹宽度,4像素对齐
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawTrace(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_TRACE_INFO_ST *pTraceInfo,unsigned int color,int lineWidth);

/** 
* @brief            图像数据格式转换
*
* @param[in]        pSrc            源图像描述，指针类型
* @param[in]        pDst            目的图像描述，指针类型
* @param[out]       none                        
* 
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_img_convertFormat(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst);

#ifdef __cplusplus
}
#endif


#endif //_OPDEVSDK_IMAGE_H_

