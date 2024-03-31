/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_jpegenc.h
* @brief        基础服务库JPEG ENCODE模块
*
* @date         2019/2/18
* @version      1.0.0
* @note         增加JPEG编码接口
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口
*
* @date         2020/2/19
* @version      2.0.0
* @note         增加注释描述信息
*
* @date         2020/2/24
* @version      2.0.1
* @note         模块名称JPEG调整为JPEGENC
*****************************************************************************/

#ifndef _OPDEVSDK_JPEGENC_H_
#define _OPDEVSDK_JPEGENC_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_img.h"
#include "opdevsdk_video.h"

#define OPDEVSDK_JPEGENC_MAX_ENC_HANDLE         (4)           /*!< JPEG编码模块支持的最大通道数量 */
#define OPDEVSDK_JPEGENC_MAX_BITMAP_CNT         (16)          /*!< JPEG编码模块支持的最大位图个数 */ 

/**
* @brief 单张位图信息参数
*/
typedef struct _OPDEVSDK_JPEGENC_BITMAP_ST_
{
	unsigned int                    pri;		    /*!< 区域叠加的优先级，值越大， 优先级越高 */
    int                             x;              /*!< 区域X轴坐标,单位:像素,2像素对齐 */
    int                             y;              /*!< 区域Y轴坐标,单位:像素,2像素对齐 */
    unsigned int                    width;          /*!< 区域宽度,单位:像素,4像素对齐 */
    unsigned int                    height;         /*!< 区域高度,单位:像素,2像素对齐 */
	unsigned int                    pitch;			/*!< 区域跨度，以像素为单位 */
	int                             bpp;			/*!< 每像素占用的字节 */
	float                           fgAlpha;        /*!< 前景透明度,[0,1], 值越小，越透明 */ 
	float                           bgAlpha;        /*!< 背景透明度,[0,1],值越小，越透明 */
	void                            *pVirAddr;	    /*!< 虚拟地址 */
	void                            *pPhyAddr;      /*!< 物理地址 */
}OPDEVSDK_JPEGENC_BITMAP_ST;

/**
* @brief 位图数据格式
*/
typedef enum _OPDEVSDK_JPEGENC_BITMAP_FMT_EN_
{
	OPDEVSDK_JPEGENC_BITMAP_ARGB1555    = 0,            /*!< RGB1555 */
	OPDEVSDK_JPEGENC_BITMAP_MAX         = OPDEVSDK_MEDIADRV_ENUM_MAXI,	
}OPDEVSDK_JPEGENC_BITMAP_FMT_EN;

/**
* @brief 位图数组
*/
typedef struct _OPDEVSDK_JPEGENC_BITMAPS_ST_
{
	 unsigned int                       bitMapCnt;                              /*!< 位图个数 */
	 OPDEVSDK_JPEGENC_BITMAP_FMT_EN     bitMapFormat;                           /*!< 位图格式 */
	 OPDEVSDK_JPEGENC_BITMAP_ST         bitMap[OPDEVSDK_JPEGENC_MAX_BITMAP_CNT];	/*!< 单张位图信息参数 */
}OPDEVSDK_JPEGENC_BITMAPS_ST;


/**
* @brief JPEG编码能力,按位使能,例如:0x3表示支持硬件编码和软件编码
*/
typedef enum _OPDEVSDK_JPEGENC_ABILI_EN_
{
    OPDEVSDK_JPEGENC_UNSUPPORT      = 0,        /*!< 不支持,无法使用JPEG编码相关接口 */
    OPDEVSDK_JPEGENC_HARDWARE       = 1,        /*!< 支持JPEG硬件编码 */
    OPDEVSDK_JPEGENC_SOFT           = 2,        /*!< 支持JPEG软件编码 */
    OPDEVSDK_JPEGENC_MAX_NUM        = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_JPEGENC_ABILI_EN;


/**
* @brief JPEG编码参数,支持对源图像裁剪 
*/
typedef struct _OPDEVSDK_JPEGENC_PARAM_ST_
{
    OPDEVSDK_IMG_RECT_PARAM_ST      rectParam;      /*!< 裁剪参数，针对源图像 */
    unsigned int                    encWidth;       /*!< 编码宽度，单位像素，16对齐 */
    unsigned int                    encHeight;      /*!< 编码高度 单位像素，8对齐 */
    unsigned int                    quality;        /*!< 编码质量,范围[1,99] */
    void                            *pBuf;          /*!< JPEG图像缓存地址,用于存储编码后图像 */
    unsigned int                    bufSize;        /*!< JPEG图像缓存大小 */
    unsigned int                    jpegSize;       /*!< 编码后实际图像大小 */
	OPDEVSDK_JPEGENC_BITMAPS_ST     bitMaps;        /*!< 编码叠加位图，不叠加bitMap_cnt 置0>*/
	int                             res[12];        /*!< 保留字段 */
	
} OPDEVSDK_JPEGENC_PARAM_ST;


/** 
* @brief            创建一路JPEG编码通道
*
* @param[out]       handle          JPEG编码句柄，指针类型
* @param[in]        bSoftEnc        软件编码标识,FALSE表示使用硬件编码;TRUE表示使用软件编码
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_create(void **handle,int bSoftEnc);


/** 
* @brief            JPEG编码处理
* @note             只支持YUV图像
*
* @param[in]        handle          JPEG编码句柄
* @param[in]        pFrame          待处理图像描述，指针类型
* @param[in/out]    pParam          JPEG编码参数，指针类型
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_proc(void *handle,OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_JPEGENC_PARAM_ST *pParam);


/** 
* @brief            销毁JPEG编码通道
*
* @param[in]        handle          JPEG编码句柄
* @param[out]       none                        
*
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_destroy(void *handle);


#ifdef __cplusplus
}
#endif

#endif //_OPDEVSDK_JPEGENC_H_


