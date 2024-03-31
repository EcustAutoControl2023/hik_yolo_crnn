/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_img.h
* @brief        ���������ͼ����ģ��
*
* @date         2019/2/18
* @version      1.0.0
* @note         ����ͼ�����ź��ڴ濽���ӿ� 
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ�
*
* @date         2020/2/19
* @version      2.0.1
* @note         ������ɫ��䣬�������򡢾��εȽӿ�
*               
*****************************************************************************/

#ifndef _OPDEVSDK_IMG_H_
#define _OPDEVSDK_IMG_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_video.h"


#define OPDEVSDK_IMG_MAX_WIDTH                  (4096)      /*!< ͼ����ģ��֧�������ͼ���� */
#define OPDEVSDK_IMG_MAX_HEIGHT                 (4096)      /*!< ͼ����ģ��֧�������ͼ��߶� */
#define OPDEVSDK_IMG_MAX_TRACE_POINT_NUM        (128)       /*!< ͼ����ģ��֧�����켣��������� */


/**
* @brief ͼ����������,��λʹ��,����0x3��ʾͬʱ֧��Ӳ�����ź��������
*/
typedef enum _OPDEVSDK_IMG_SCALE_ABILI_EN_
{
    OPDEVSDK_IMG_SCALE_UNSUPPORT        = 0,        /*!< ��֧������,�޷�ʹ��ͼ��������ؽӿ� */
    OPDEVSDK_IMG_SCALE_HARDWARE         = 1,        /*!< ֧��Ӳ������,��ʹ��Ӳ�����Žӿ�opdevsdk_img_scale */
    OPDEVSDK_IMG_SCALE_SOFT             = 2,        /*!< ֧���������,��ʹ��������Žӿ�opdevsdk_img_softScale */
    OPDEVSDK_IMG_SCALE_MAX_NUM          = OPDEVSDK_MEDIADRV_ENUM_MAXI, 
}OPDEVSDK_IMG_SCALE_ABILI_EN;


/**
* @brief ���������
*/
typedef struct _OPDEVSDK_IMG_POINT_ST_
{
    int       x;      /*!< X������,��λ:����,2���ض��� */
    int       y;      /*!< Y������,��λ:����,2���ض��� */
}OPDEVSDK_IMG_POINT_ST;


/**
* @brief ���ο�����
*/
typedef struct _OPDEVSDK_IMG_RECT_PARAM_ST_
{
    OPDEVSDK_IMG_POINT_ST       point;          /*!< ���ο����Ͻ��������Ϣ */
    unsigned int                width;          /*!< ���ο���,��λ:����,4���ض��� */
    unsigned int                height;         /*!< ���ο�߶�,��λ:����,2���ض��� */
}OPDEVSDK_IMG_RECT_PARAM_ST;


/**
* @brief ���Ų�������
*/
typedef struct _OPDEVSDK_IMG_SCALE_PARAM_ST_
{
    OPDEVSDK_IMG_RECT_PARAM_ST      rectParam;      /*!< ѡ�����������,�������ͼ�� */ 
    int                             res[4];         /*!< Ԥ���ֶ� */
}OPDEVSDK_IMG_SCALE_PARAM_ST;


/**
* @brief �켣����㼯������
*/
typedef struct _OPDEVSDK_IMG_TRACE_INFO_ST_
{
    unsigned int                pointNum;       /*!< �켣��������� */ 
    OPDEVSDK_IMG_POINT_ST       *pPoint;        /*!< ÿ���켣�����������ָ������;
                                                 *��pointNum = N����ָ��pPointָ��ĵ�ַ����ΪN*sizeof(OPDEVSDK_IMG_POINT_ST)*/
}OPDEVSDK_IMG_TRACE_INFO_ST;


/** 
* @brief            ���ݿ����ӿ�,ʹ��dmaӲ������
* @note             ע�������������������ַ���Դ�������memcpy
*
* @param[in]        pPhySrc         Դͼ�����������ַ
* @param[in]        pPhyDst         Ŀ��ͼ�����������ַ
* @param[in]        size            �������ݳ���
* @param[out]       ��                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaCopy1d(void *pPhySrc,void *pPhyDst,unsigned int size);


/** 
* @brief            ͼ�����ݿ����ӿ�,ʹ��dmaӲ������
* @note             ͼ�����ڶ�ά��������һά����
*
* @param[in]        pSrc            Դͼ��������ָ������
* @param[in]        pDst            Ŀ��ͼ��������ָ������
* @param[in]        param           ��Դͼ��ü�����ѡ������
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaCopy2d(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_RECT_PARAM_ST param);


/** 
* @brief            ������Žӿ�
* @note             ARM�汾���ţ�������Ӳ������
*
* @param[in]        pSrc            Դͼ��������ָ������
* @param[in]        pDst            Ŀ��ͼ��������ָ������
* @param[in]        param           ���Ų���
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_softScale(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_SCALE_PARAM_ST param);


/** 
* @brief            Ӳ�����Žӿ�
*
* @param[in]        pSrc            Դͼ��������ָ������
* @param[in]        pDst            Ŀ��ͼ��������ָ������
* @param[in]        param           ���Ų���
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_scale(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst,OPDEVSDK_IMG_SCALE_PARAM_ST param);


/** 
* @brief            ͼ�����ɫ��
* @note             ��֧��YUVͼ��
*
* @param[in]        pFrame          ������ͼ��������ָ������
* @param[in]        param           ������������ο�
* @param[in]        color           �������ɫ,�ο�����OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_dmaFill(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_RECT_PARAM_ST param,unsigned int color);


/** 
* @brief            ͼ���������
* @note             ��֧��YUVͼ��,������㡢�յ�����2���ض���,ˮƽ����Ҫ����4���ض���
*
* @param[in]        pFrame          ������ͼ��������ָ������
* @param[in]        src             �����������
* @param[in]        dst             �����յ�����
* @param[in]        color           ������ɫ,�ο�����OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       �������,4���ض���
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawLine(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_POINT_ST src,OPDEVSDK_IMG_POINT_ST dst,unsigned int color,int lineWidth);


/** 
* @brief            ͼ����ƾ��ο�
* @note             ��֧��YUVͼ��
*
* @param[in]        pFrame          ������ͼ��������ָ������
* @param[in]        param           �������������
* @param[in]        color           ���ο���ɫ,�ο�����OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       ���ο���������,4���ض���
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawRect(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_RECT_PARAM_ST param,unsigned int color,int lineWidth);


/** 
* @brief            ͼ����ƹ켣
* @note             ��֧��YUVͼ��
*
* @param[in]        pFrame          ������ͼ��������ָ������
* @param[in]        pTraceInfo      �켣������ָ������
* @param[in]        color           �켣��ɫ,�ο�����OPDEVSDK_MEDIADRV_MAKE_COLOR(y,u,v)
* @param[in]        lineWidth       �켣���,4���ض���
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_drawTrace(OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_IMG_TRACE_INFO_ST *pTraceInfo,unsigned int color,int lineWidth);

/** 
* @brief            ͼ�����ݸ�ʽת��
*
* @param[in]        pSrc            Դͼ��������ָ������
* @param[in]        pDst            Ŀ��ͼ��������ָ������
* @param[out]       none                        
* 
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_img_convertFormat(OPDEVSDK_VIDEO_FRAME_ST *pSrc,OPDEVSDK_VIDEO_FRAME_ST *pDst);

#ifdef __cplusplus
}
#endif


#endif //_OPDEVSDK_IMAGE_H_

