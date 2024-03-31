/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_video.h
* @brief        �����������Ƶ��ͷ�ļ�
*
* @date         2020/8/13
* @version      2.0.0
* @note         ������Ƶ��ؽṹ��
*****************************************************************************/

#ifndef _OPDEVSDK_VIDEO_H_
#define _OPDEVSDK_VIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"


/**
* @brief YUVͼ��洢��ʽ
*/
typedef enum _OPDEVSDK_VIDEO_PIXEL_FORMAT_EN_
{
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YUV422     = 0,         /*!< YUV4:2:2,YUV����洢,����:YUYV */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_NV12       = 1,         /*!< YUV4:2:0,Yƽ��洢,UV����洢,��U��ǰ */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_NV21       = 2,         /*!< YUV4:2:0,Yƽ��洢,UV����洢,��V��ǰ,Ĭ�ϸ�ʽ */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YUV400     = 3,         /*!< YUV4:0:0,��Y���� */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_I420       = 4,         /*!< YUV4:2:0,I420Ҳ��YU12,Y,U,V����������ƽ�洢�� */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_YV12       = 5,         /*!< YUV4:2:0,Y,V,U����������ƽ�洢�� */
    OPDEVSDK_VIDEO_PIXEL_FORMAT_MAX_NUM    = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_PIXEL_FORMAT_EN;


/**
* @brief  ͼ�����ݸ�ʽ
*/
typedef enum _OPDEVSDK_VIDEO_DATA_TYPE_EN_
{
    OPDEVSDK_VIDEO_DATA_TYPE_VI_FRAME             = 0,      /*!< ������Ƶ֡ */
    OPDEVSDK_VIDEO_DATA_TYPE_CAPTURE_IMAGE        = 1,      /*!< ����ץ��֡ */
    OPDEVSDK_VIDEO_DATA_TYPE_CUSTOM               = 2,      /*!< ����֡ */
    OPDEVSDK_VIDEO_DATA_TYPE_MAX_NUM              = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_DATA_TYPE_EN;


/**
* @brief  ��Ƶ֡����Ϣ
*/
typedef enum _OPDEVSDK_VIDEO_FRAME_RATE_EN_
{
    OPDEVSDK_VIDEO_FRAME_RATE_FULL      = 0,        /*!< ��Ƶȫ֡��(���֡��) */
    OPDEVSDK_VIDEO_FRAME_RATE_1         = 1,        /*!< ��Ƶ֡��:1 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_6_25      = 2,        /*!< ��Ƶ֡��:6.25 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_8         = 3,        /*!< ��Ƶ֡��:8 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_10        = 4,        /*!< ��Ƶ֡��:10 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_12_5      = 5,        /*!< ��Ƶ֡��:12.5 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_15        = 6,        /*!< ��Ƶ֡��:15 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_20        = 7,        /*!< ��Ƶ֡��:20 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_25        = 8,        /*!< ��Ƶ֡��:25 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_30        = 9,        /*!< ��Ƶ֡��:30 fps */
    OPDEVSDK_VIDEO_FRAME_RATE_MAX_NUM   = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_VIDEO_FRAME_RATE_EN;

/**
* @brief  ͼ��ѹ��ģʽ
*/
typedef enum _OPDEVSDK_COMPRESS_MODE_EN_
{
    OPDEVSDK_COMPRESS_MODE_NONE         = 0,        /*!< ��ѹ�� */
    OPDEVSDK_COMPRESS_MODE_MAX_NUM      = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
} OPDEVSDK_COMPRESS_MODE_EN;

/**
* @brief һ֡ͼ������
*/
typedef struct _OPDEVSDK_VIDEO_FRAME_ST_
{
    OPDEVSDK_VIDEO_PIXEL_FORMAT_EN      dataFormat;         /*!< ͼ��洢��ʽ,����ο�OPDEVSDK_VIN_DATA_FORMAT_EN */
    int                                 width;              /*!< ͼ����,��С16���ض��� */
    int                                 height;             /*!< ͼ��߶�,��С8���ض��� */
    int                                 pitch[3];           /*!< ͼ�������� */
    void                                *pVirAddr[3];       /*!< ͼ����������ַ */
    void                                *pPhyAddr[3];       /*!< ͼ����������ַ */

    OPDEVSDK_COMPRESS_MODE_EN 			enCompressMode;		/*!< ͼ��ѹ����ʽ */
    int                              	res[5];        	/*!< Ԥ���ֶ� */

} OPDEVSDK_VIDEO_FRAME_ST;


/**
* @brief һ֡ͼ����������(����Я������)
*/
typedef struct _OPDEVSDK_VIDEO_FRAME_INFO_ST_
{
    unsigned int                        chan;           /*!< ͨ���� */
    unsigned long long                  timeStamp;      /*!< ʱ���,��λ:΢�� */
    unsigned int                        frmNum;         /*!< ͼ��֡�� */
    OPDEVSDK_VIDEO_DATA_TYPE_EN         dataType;       /*!< ͼ�����ݸ�ʽ,����ο�OPDEVSDK_VIDEO_DATA_TYPE_EN */
    unsigned int                        dataLen;        /*!< ˽�����ݳ���,����dataType=OPDEVSDK_VIDEO_DATA_TYPE_CUSTOMģʽ����Ч */

    OPDEVSDK_MEDIADRV_TIME_ST           dateTime;       /*!< ͼ��֡ȫ��ʱ�� */
    OPDEVSDK_VIDEO_FRAME_ST             yuvFrame;       /*!< һ֡ͼ������ */
    int                                 res[6];        /*!< Ԥ���ֶ� */
} OPDEVSDK_VIDEO_FRAME_INFO_ST;


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_VIDEO_H_ */


