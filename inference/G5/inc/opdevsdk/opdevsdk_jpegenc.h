/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_jpegenc.h
* @brief        ���������JPEG ENCODEģ��
*
* @date         2019/2/18
* @version      1.0.0
* @note         ����JPEG����ӿ�
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ�
*
* @date         2020/2/19
* @version      2.0.0
* @note         ����ע��������Ϣ
*
* @date         2020/2/24
* @version      2.0.1
* @note         ģ������JPEG����ΪJPEGENC
*****************************************************************************/

#ifndef _OPDEVSDK_JPEGENC_H_
#define _OPDEVSDK_JPEGENC_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "opdevsdk_mediadrv_comm.h"
#include "opdevsdk_img.h"
#include "opdevsdk_video.h"

#define OPDEVSDK_JPEGENC_MAX_ENC_HANDLE         (4)           /*!< JPEG����ģ��֧�ֵ����ͨ������ */
#define OPDEVSDK_JPEGENC_MAX_BITMAP_CNT         (16)          /*!< JPEG����ģ��֧�ֵ����λͼ���� */ 

/**
* @brief ����λͼ��Ϣ����
*/
typedef struct _OPDEVSDK_JPEGENC_BITMAP_ST_
{
	unsigned int                    pri;		    /*!< ������ӵ����ȼ���ֵԽ�� ���ȼ�Խ�� */
    int                             x;              /*!< ����X������,��λ:����,2���ض��� */
    int                             y;              /*!< ����Y������,��λ:����,2���ض��� */
    unsigned int                    width;          /*!< ������,��λ:����,4���ض��� */
    unsigned int                    height;         /*!< ����߶�,��λ:����,2���ض��� */
	unsigned int                    pitch;			/*!< �����ȣ�������Ϊ��λ */
	int                             bpp;			/*!< ÿ����ռ�õ��ֽ� */
	float                           fgAlpha;        /*!< ǰ��͸����,[0,1], ֵԽС��Խ͸�� */ 
	float                           bgAlpha;        /*!< ����͸����,[0,1],ֵԽС��Խ͸�� */
	void                            *pVirAddr;	    /*!< �����ַ */
	void                            *pPhyAddr;      /*!< �����ַ */
}OPDEVSDK_JPEGENC_BITMAP_ST;

/**
* @brief λͼ���ݸ�ʽ
*/
typedef enum _OPDEVSDK_JPEGENC_BITMAP_FMT_EN_
{
	OPDEVSDK_JPEGENC_BITMAP_ARGB1555    = 0,            /*!< RGB1555 */
	OPDEVSDK_JPEGENC_BITMAP_MAX         = OPDEVSDK_MEDIADRV_ENUM_MAXI,	
}OPDEVSDK_JPEGENC_BITMAP_FMT_EN;

/**
* @brief λͼ����
*/
typedef struct _OPDEVSDK_JPEGENC_BITMAPS_ST_
{
	 unsigned int                       bitMapCnt;                              /*!< λͼ���� */
	 OPDEVSDK_JPEGENC_BITMAP_FMT_EN     bitMapFormat;                           /*!< λͼ��ʽ */
	 OPDEVSDK_JPEGENC_BITMAP_ST         bitMap[OPDEVSDK_JPEGENC_MAX_BITMAP_CNT];	/*!< ����λͼ��Ϣ���� */
}OPDEVSDK_JPEGENC_BITMAPS_ST;


/**
* @brief JPEG��������,��λʹ��,����:0x3��ʾ֧��Ӳ��������������
*/
typedef enum _OPDEVSDK_JPEGENC_ABILI_EN_
{
    OPDEVSDK_JPEGENC_UNSUPPORT      = 0,        /*!< ��֧��,�޷�ʹ��JPEG������ؽӿ� */
    OPDEVSDK_JPEGENC_HARDWARE       = 1,        /*!< ֧��JPEGӲ������ */
    OPDEVSDK_JPEGENC_SOFT           = 2,        /*!< ֧��JPEG������� */
    OPDEVSDK_JPEGENC_MAX_NUM        = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_JPEGENC_ABILI_EN;


/**
* @brief JPEG�������,֧�ֶ�Դͼ��ü� 
*/
typedef struct _OPDEVSDK_JPEGENC_PARAM_ST_
{
    OPDEVSDK_IMG_RECT_PARAM_ST      rectParam;      /*!< �ü����������Դͼ�� */
    unsigned int                    encWidth;       /*!< �����ȣ���λ���أ�16���� */
    unsigned int                    encHeight;      /*!< ����߶� ��λ���أ�8���� */
    unsigned int                    quality;        /*!< ��������,��Χ[1,99] */
    void                            *pBuf;          /*!< JPEGͼ�񻺴��ַ,���ڴ洢�����ͼ�� */
    unsigned int                    bufSize;        /*!< JPEGͼ�񻺴��С */
    unsigned int                    jpegSize;       /*!< �����ʵ��ͼ���С */
	OPDEVSDK_JPEGENC_BITMAPS_ST     bitMaps;        /*!< �������λͼ��������bitMap_cnt ��0>*/
	int                             res[12];        /*!< �����ֶ� */
	
} OPDEVSDK_JPEGENC_PARAM_ST;


/** 
* @brief            ����һ·JPEG����ͨ��
*
* @param[out]       handle          JPEG��������ָ������
* @param[in]        bSoftEnc        ��������ʶ,FALSE��ʾʹ��Ӳ������;TRUE��ʾʹ���������
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_create(void **handle,int bSoftEnc);


/** 
* @brief            JPEG���봦��
* @note             ֻ֧��YUVͼ��
*
* @param[in]        handle          JPEG������
* @param[in]        pFrame          ������ͼ��������ָ������
* @param[in/out]    pParam          JPEG���������ָ������
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_proc(void *handle,OPDEVSDK_VIDEO_FRAME_ST *pFrame,OPDEVSDK_JPEGENC_PARAM_ST *pParam);


/** 
* @brief            ����JPEG����ͨ��
*
* @param[in]        handle          JPEG������
* @param[out]       none                        
*
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_jpegenc_destroy(void *handle);


#ifdef __cplusplus
}
#endif

#endif //_OPDEVSDK_JPEGENC_H_


