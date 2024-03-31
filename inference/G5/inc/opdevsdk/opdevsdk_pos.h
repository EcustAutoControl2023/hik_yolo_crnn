/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_pos.h
* @brief        ������POS(play on screen)ģ��
* @note         ֻ�����ڽ���Ϣ�����������
*
* @date         2019/5/16
* @version      1.0.0
* @note         ����luma/expose����ӿ�
*
* @date         2019/6/1
* @version      2.0.0
* @note         ͬ�������ڲ��ӿ�
*
* @date         2020/2/19
* @version      2.0.1
* @note         ��Ŀ�������Ϊָ��(target list��rule list��luma)��������չĿ������
*
* @date         2020/3/20
* @version      2.0.2
* @note         1.����procType������ͽӿ�,
*               2.��luma��expose�ӿ�����ISPģ��
*****************************************************************************/

#ifndef _OPDEVSDK_POS_H_
#define _OPDEVSDK_POS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"


#define OPDEVSDK_POS_MAX_RULE_NUM           (64)        /*!< ������������� */
#define OPDEVSDK_POS_MAX_TARGET_NUM         (512)       /*!< Ŀ����������� */
#define OPDEVSDK_POS_MAX_ALERT_NUM          (64)        /*!< �������������� */
#define OPDEVSDK_POS_MAX_RULE_VERTEX_NUM    (10)        /*!< ����������ඥ������ */
#define OPDEVSDK_POS_MAX_TARGET_VERTEX_NUM  (10)        /*!< Ŀ��������ඥ������ */
#define OPDEVSDK_POS_MAX_TEXT_LINE_NUM      (32)        /*!< Ԥ��pos����������� */
#define OPDEVSDK_POS_MAX_CHAR_PER_LINE      (44)        /*!< Ԥ��posÿ�е����ַ�������� */

#define OPDEVSDK_POS_PACKAGE_ID_BASE        (0)         /*!< ���ID,����������ͬʱ�����ͬ���ܵ�˽����Ϣ */

#define OPDEVSDK_POS_MAX_PRIV_DATA_LEN      (32)        /*!< ���˽�����ݳ��� */

/*!< ����RGB��ɫ,type����ο�OPDEVSDK_POS_RGB_COLOR_TYPE_EN */
#define OPDEVSDK_POS_MAKE_RGB_COLOR(type,b,g,r)     (((type&0xff)<<24)|((b&0xff)<<16)|((g&0xff)<<8)|(r&0xff))

/**
* @brief �Զ���Ŀ�ꡢ���򡢹켣��ɫ���
*/
typedef enum _OPDEVSDK_POS_RGB_COLOR_TYPE_EN_
{
    OPDEVSDK_POS_RGB_COLOR_TYPE_NONE                = 0,        /*!< δ����,ʹ��Ĭ����ɫ */
    OPDEVSDK_POS_RGB_COLOR_TYPE_RULE                = 1,        /*!< ������Զ�����ɫ��� */
    OPDEVSDK_POS_RGB_COLOR_TYPE_TARGET              = 2,        /*!< Ŀ����Զ�����ɫ��� */
    OPDEVSDK_POS_RGB_COLOR_TYPE_HUMAN_TRACE         = 3,        /*!< ����켣�Զ�����ɫ��� */
    OPDEVSDK_POS_RGB_COLOR_TYPE_VEHICLE_TRACE       = 4,        /*!< �������켣�Զ�����ɫ��� */
    OPDEVSDK_POS_RGB_COLOR_TYPE_MAX                 = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
}OPDEVSDK_POS_RGB_COLOR_TYPE_EN;

/**
* @brief �����������
*/
typedef struct _OPDEVSDK_POS_POINT_ST_
{
    float       x;      /*!< X���һ������,��Χ[0,1] */
    float       y;      /*!< Y���һ������,��Χ[0,1] */
}OPDEVSDK_POS_POINT_ST;


/**
* @brief ����Ŀ����������
*/
typedef struct _OPDEVSDK_POS_RECT_ST_
{
    float       x;          /*!< X���һ������,��Χ[0,1] */
    float       y;          /*!< Y���һ������,��Χ[0,1] */
    float       width;      /*!< X���һ�����,��Χ(0,1] */
    float       height;     /*!< X���һ���߶�,��Χ(0,1] */
}OPDEVSDK_POS_RECT_ST;


/**
* @brief Ŀ����������
*/
typedef struct _OPDEVSDK_POS_REGION_ST_
{
    unsigned int                pointNum;                                       /*!< �����Ӧ��������� */ 
    OPDEVSDK_POS_POINT_ST       point[OPDEVSDK_POS_MAX_TARGET_VERTEX_NUM];      /*!< ����������������� */
}OPDEVSDK_POS_REGION_ST;


/**
* @brief Ŀ����������
*/
typedef enum _OPDEVSDK_POS_REGION_TYPE_EN_
{
    OPDEVSDK_POS_REGION_NONE                = 0,            /*!< δ�������� */
    OPDEVSDK_POS_REGION_POLYLINE            = 1,            /*!< �������� */
    OPDEVSDK_POS_REGION_POLYGON             = 2,            /*!< ����� */
    OPDEVSDK_POS_REGION_RECT                = 3,            /*!< ���� */
    OPDEVSDK_POS_REGION_MAX                 = OPDEVSDK_MEDIADRV_ENUM_MAXI 
}OPDEVSDK_POS_REGION_TYPE_EN;


/**
* @brief Ŀ����������
*/
typedef struct _OPDEVSDK_POS_REGION_V2_ST_
{
    unsigned int    regionType;                             /*!< �������ͣ��ο�OPDEVSDK_POS_REGION_TYPE_EN */
	char            res[12];                                /*!< �����ֶ� */
	union
	{
		unsigned char		            size[84];           /*!< �����ֶΣ�ȷ���������С */
		OPDEVSDK_POS_REGION_ST          polygon;            /*!< ����Ρ�ֱ�ߡ�����Ŀ����� */
		OPDEVSDK_POS_RECT_ST            rect;               /*!< ����Ŀ����� */
	};
}OPDEVSDK_POS_REGION_V2_ST;


/**
* @brief �������ʾ����(��ɫ����ȡ���ʽ��)
*/
typedef enum _OPDEVSDK_POS_POLYGON_ATTRI_EN_
{
    OPDEVSDK_POS_POLYGON_ATTRI_NORMAL               = 0,        /*!< ��ͨ��������,��ɫĿ���,��ɫ�����,�������ɫ */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBG_ONE_HALF      = 1,        /*!< �����,��ͨ��ɫ1/2 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBR_ONE_HALF      = 2,        /*!< Ŀ���,�Ӵֺ�ɫ1/2 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBG_TWO_THIRD     = 3,        /*!< �����,��ͨ��ɫ2/3 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBR_TWO_THIRD     = 4,        /*!< Ŀ���,�Ӵֺ�ɫ2/3 */
    OPDEVSDK_POS_POLYGON_ATTRI_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
} OPDEVSDK_POS_POLYGON_ATTRI_EN;


/**
* @brief ʱ������ͣ���������RTSP��������YUV����Я����ʱ���
*/
typedef enum _OPDEVSDK_POS_TIME_TYPE_EN_
{
    OPDEVSDK_POS_TIME_TYPE_1K                   = 0,        /*!< ʱ���,��λ:1KB������Ϊ���� */
    OPDEVSDK_POS_TIME_TYPE_90K                  = 1,        /*!< ʱ���,��λ:90KB */
    OPDEVSDK_POS_TIME_TYPE_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
} OPDEVSDK_POS_TIME_TYPE_EN;


/**
* @brief ��Ŀ������
*/
typedef struct _OPDEVSDK_POS_TARGET_ST_
{
    unsigned int                id;             /*!< Ŀ��ID */
    unsigned int                trace_time;     /*!< �켣��ʾʱ��,Ŀ���Ϊ�켣����ʱ��Ч,��Χ(0,60),Ĭ��30,��λ�� */
    unsigned int                color;          /*!< ��ɫ��Ϣ,RGB��ʽ ������color��Чʱ��OPDEVSDK_POS_POLYGON_ATTRI_EN�����ɫ����ʧЧ */
    OPDEVSDK_POS_REGION_ST      region;         /*!< Ŀ��������Ϣ */	
    int                         res[4];         /*!< �����ֶ� */
}OPDEVSDK_POS_TARGET_ST;


/**
* @brief ��Ŀ������
*/
typedef struct _OPDEVSDK_POS_TARGET_V2_ST_
{
    unsigned int                    id;             /*!< Ŀ��ID */
    OPDEVSDK_POS_REGION_V2_ST       region;         /*!< Ŀ��������Ϣ */	
    int                             res[4];         /*!< �����ֶ� */
}OPDEVSDK_POS_TARGET_V2_ST;


/**
* @brief Ŀ����������
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_ST_
{
    unsigned int                tgtNum;         /*!< Ŀ������,��Χ[0,OPDEVSDK_POS_MAX_TARGET_NUM] */
    OPDEVSDK_POS_TARGET_ST      *pTgt;          /*!< ����Ŀ������ */
}OPDEVSDK_POS_TARGET_LIST_ST;


/**
* @brief ������ͬ����(ʱ�������ɫ����ȡ���ʽ��)��Ŀ����Ϣ����
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< ʱ������ͣ�ͨ������������ʽ��ȡ���������ݵģ��������90Kʱ��� */
    unsigned long long                  timeStamp;      /*!< ʱ��� */
    unsigned int                        packId;         /*!< ���ID�������������ͣ���0��ʼ */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< Ŀ�����ʾ���� */
    OPDEVSDK_POS_TARGET_LIST_ST         tgtList;        /*!< Ŀ������ */
    int                                 res[8];         /*!< �����ֶ� */    
}OPDEVSDK_POS_TARGET_LIST_INFO_ST;


/**
* @brief ��ɫ��Ϣ
*/
typedef struct _OPDEVSDK_POS_EX_INFO_COLOR_ST_
{
    unsigned char red;          /*!< ��ɫ���� */
    unsigned char green;        /*!< ��ɫ���� */
    unsigned char blue;         /*!< ��ɫ���� */
    unsigned char alpha;        /*!< alphaɫ�ʿռ䣬��δʹ�� */
}OPDEVSDK_POS_EX_INFO_COLOR_ST;

/**
* @brief �Զ���켣���
*/
typedef enum _OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN_
{
    OPDEVSDK_POS_EX_TRACE_TYPE_NONE                 = 0,        /*!< δ���� */
    OPDEVSDK_POS_EX_TRACE_TYPE_HUMAN_TRACE          = 1,        /*!< ����켣��� */
    OPDEVSDK_POS_EX_TRACE_TYPE_VEHICLE_TRACE        = 2,        /*!< �������켣��� */
    OPDEVSDK_POS_EX_TRACE_TYPE_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
}OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN;

/**
* @brief ��ɫ�켣��Ϣ
*/
typedef struct _OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST_
{
    OPDEVSDK_POS_EX_INFO_COLOR_ST           color;          /*!< ��ɫ��Ϣ */
    unsigned char                           lineTime;       /*!< �켣����ʱ�䣬��Χ(0,60)����λ:�� */
    unsigned char                           reseverd[3];    /*!< �����ֶ� */
    OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN      targetType;     /*!< Ŀ��켣���ͣ�1-�˹켣��2-���켣 */
}OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST;


/**
* @brief ˽����չ��Ϣ����
*/
typedef enum _OPDEVSDK_POS_EX_INFO_TYPE_EN_
{
    OPDEVSDK_POS_EX_INFO_NONE               = 0,            /*!< ����չ��Ϣ */
    OPDEVSDK_POS_EX_INFO_COLOR              = 1,            /*!< ��չ��ɫ��Ϣ(��չ��������OPDEVSDK_POS_EX_INFO_COLOR_ST) */
    OPDEVSDK_POS_EX_INFO_COLOR_LINE         = 2,            /*!< ��ɫ�켣��Ϣ(��չ��������OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST) */
    OPDEVSDK_POS_EX_INFO_SPEED_COORD        = 3,            /*!< �ٶ�������Ϣ(�ݲ�֧��) */
    OPDEVSDK_POS_EX_INFO_COORD_S            = 4,            /*!< ��γ����Ϣ���ݲ�֧�֣� */
    OPDEVSDK_POS_EX_INFO_EAGLE_R            = 5,            /*!< ӥ�۸��ٺ�ɫ�Ľǿ�(����չ����) */
    OPDEVSDK_POS_EX_INFO_MAX                = OPDEVSDK_MEDIADRV_ENUM_MAXI 
}OPDEVSDK_POS_EX_INFO_TYPE_EN;


/**
* @brief ˽����չ��Ϣ����
*/
typedef struct _OPDEVSDK_POS_EX_INFO_ST_
{
    unsigned char                       exType;                                     /*!< ��չ��Ϣ���Ͳο�OPDEVSDK_POS_EX_INFO_TYPE_EN */
    unsigned char                       reseverd[14];                               /*!< �����ֶ� */
    unsigned char                       exLen;                                      /*!< ��չ��Ϣ���� */
    unsigned char                       exData[OPDEVSDK_POS_MAX_PRIV_DATA_LEN];     /*!< ��չ���ݣ�����������exType��� */
}OPDEVSDK_POS_EX_INFO_ST;


/**
* @brief Ŀ����Ϣ������֧����չ��Ϣ
*/
typedef struct _OPDEVSDK_POS_TARGET_EX_ST_
{
    OPDEVSDK_POS_TARGET_V2_ST           tgt;            /*!< Ŀ����Ϣ */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< ��չ��Ϣ */
}OPDEVSDK_POS_TARGET_EX_ST;


/**
* @brief Ŀ����������
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_V2_ST_
{
    unsigned int                    tgtNum;         /*!< Ŀ������,��Χ[0,OPDEVSDK_POS_MAX_TARGET_NUM] */
    OPDEVSDK_POS_TARGET_EX_ST       *pTgt;          /*!< ����Ŀ������ */
}OPDEVSDK_POS_TARGET_LIST_V2_ST;


/**
* @brief ������ͬ����(ʱ�����)��Ŀ����Ϣ����
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< ʱ������ͣ�ͨ������������ʽ��ȡ���������ݵģ��������90Kʱ��� */
    unsigned long long                  timeStamp;      /*!< ʱ��� */
    unsigned int                        packId;         /*!< ���ID�������������ͣ���0��ʼ */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< Ŀ�����ʾ���� */
    OPDEVSDK_POS_TARGET_LIST_V2_ST      tgtList;        /*!< Ŀ������ */
    int                                 res[8];         /*!< �����ֶ� */    
}OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST;


/**
* @brief ��������
*/
typedef enum _OPDEVSDK_POS_RULE_TYPE_EN_
{
    OPDEVSDK_POS_RULE_TYPE_LINE         = 0,        /*!< �߹��� ��Ӧ�ó�������ߵ� */
    OPDEVSDK_POS_RULE_TYPE_REGION       = 1,        /*!< �������Ӧ�ó������������֣��뿪����� */
    OPDEVSDK_POS_RULE_TYPE_MAX_NUM      = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_POS_RULE_TYPE_EN;


/**
* @brief ������������
*/
typedef struct _OPDEVSDK_POS_RULE_POLYGON_ST_
{
    unsigned int                pointNum;                                   /*!< ������������� */
    OPDEVSDK_POS_POINT_ST       point[OPDEVSDK_POS_MAX_RULE_VERTEX_NUM];    /*!< �������Ϣ */
}OPDEVSDK_POS_RULE_POLYGON_ST;


/**
* @brief ����������
*/
typedef struct _OPDEVSDK_POS_RULE_ST_
{
    unsigned int                        id;             /*!< ����ID */
    int                                 enable;         /*!< ����ʹ��,TRUE��ʾʹ��,FALSE��ʾ��ʹ�� */
    OPDEVSDK_POS_RULE_TYPE_EN           ruleType;       /*!< �������� */
    OPDEVSDK_POS_RULE_POLYGON_ST        polygon;        /*!< ����������� */
    unsigned int                        color;          /*!< ��ɫ��Ϣ,RGB��ʽ,����չ��Ϣ(exInfo)ʱ��Ч,�ο�OPDEVSDK_POS_MAKE_RGB_COLOR */
    int                                 reserved[3];    /*!< �����ֶ� */
}OPDEVSDK_POS_RULE_ST;


/**
* @brief ������������
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_ST_
{
    unsigned int                        ruleNum;        /*!< ��������,��Χ[0,OPDEVSDK_POS_MAX_RULE_NUM] */
    OPDEVSDK_POS_RULE_ST                *pRule;         /*!< ������Ϣ */
}OPDEVSDK_POS_RULE_LIST_ST;


/**
* @brief ������ͬ����(ʱ�������ɫ����ȡ���ʽ��)�Ĺ�����Ϣ����
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< ʱ������� */
    unsigned long long                  timeStamp;      /*!< ʱ��� */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< �������ʾ���� */
    OPDEVSDK_POS_RULE_LIST_ST           ruleList;       /*!< �������� */
    unsigned int                        packId;         /*!< ���ID�������������ͣ���0��ʼ */
}OPDEVSDK_POS_RULE_LIST_INFO_ST;


/**
* @brief ������Ϣ������֧����չ��Ϣ
*/
typedef struct _OPDEVSDK_POS_RULE_V2_ST_
{
    OPDEVSDK_POS_RULE_ST                rule;           /*!< ������Ϣ */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< ��չ��Ϣ */
}OPDEVSDK_POS_RULE_V2_ST;


/**
* @brief ������������
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_V2_ST_
{
    unsigned int                        ruleNum;        /*!< ��������,��Χ[0,OPDEVSDK_POS_MAX_RULE_NUM] */
    OPDEVSDK_POS_RULE_V2_ST             *pRule;         /*!< ������Ϣ */
}OPDEVSDK_POS_RULE_LIST_V2_ST;


/**
* @brief ������ͬ����(ʱ�����)�Ĺ�����Ϣ����
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< ʱ������� */
    unsigned long long                  timeStamp;      /*!< ʱ��� */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< �������ʾ���� */
    OPDEVSDK_POS_RULE_LIST_V2_ST        ruleList;       /*!< �������� */
    unsigned int                        packId;         /*!< ���ID�������������ͣ���0��ʼ */
}OPDEVSDK_POS_RULE_LIST_INFO_V2_ST;


/**
* @brief ������Ϣ���
*/
typedef struct  _OPDEVSDK_POS_ALERT_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< ʱ������� */
    unsigned long long              timeStamp;          /*!< ʱ��� */
    unsigned char                   alert;              /*!< ������־,0��ʾ�ޱ���,1��ʾ���� */
    unsigned char                   viewState;          /*!< ����״̬,0��ʾ����,1��ʾ�������,2��ʾ����ڵ� */
    unsigned char                   res[6];             /*!< �����ֶ� */
    OPDEVSDK_POS_RULE_ST            ruleInfo;           /*!< ����������Ϣ */
    OPDEVSDK_POS_TARGET_ST          target;             /*!< ����Ŀ����Ϣ */ 
    unsigned int                    packId;             /*!< ���ID�������������ͣ���0��ʼ */
}OPDEVSDK_POS_ALERT_INFO_ST;

/**
* @brief ����Ŀ�ꡢ����������֧����չ��Ϣ
*/
typedef struct _OPDEVSDK_POS_ALERT_V2_ST_
{
    OPDEVSDK_POS_RULE_ST                rule;           /*!< ������Ϣ */
    OPDEVSDK_POS_TARGET_V2_ST           tgt;            /*!< Ŀ����Ϣ */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< ��չ��Ϣ */
}OPDEVSDK_POS_ALERT_V2_ST;

/**
* @brief ����Ŀ�ꡢ����������֧����չ��Ϣ
*/
typedef struct _OPDEVSDK_POS_ALERT_LIST_V2_ST_
{
    unsigned int                        alertNum;       /*!< ��������,��Χ[0,OPDEVSDK_POS_MAX_ALERT_NUM] */
    OPDEVSDK_POS_ALERT_V2_ST            *pAlert;        /*!< ������Ϣ */
}OPDEVSDK_POS_ALERT_LIST_V2_ST;

/**
* @brief ������Ϣ���
*/
typedef struct  _OPDEVSDK_POS_ALERT_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< ʱ������� */
    unsigned long long              timeStamp;          /*!< ʱ��� */
    unsigned char                   alert;              /*!< ������־,0��ʾ�ޱ���,1��ʾ���� */
    unsigned char                   viewState;          /*!< ����״̬,0��ʾ����,1��ʾ�������,2��ʾ����ڵ� */
    unsigned char                   res[6];             /*!< �����ֶ� */
    OPDEVSDK_POS_ALERT_LIST_V2_ST   alertList;          /*!< ������Ϣ */ 
    unsigned int                    packId;             /*!< ���ID�������������ͣ���0��ʼ */
}OPDEVSDK_POS_ALERT_INFO_V2_ST;


/**
* @brief �ַ�����Ϣ
*/
typedef struct  _OPDEVSDK_POS_STRING_ST_
{
    OPDEVSDK_POS_POINT_ST           point;              /*!< pos���� */
    char                            *str;               /*!< �ַ��� */
    unsigned int                    strLen;             /*!< �ַ�������,��Χ[0,OPDEVSDK_POS_MAX_CHAR_PER_LINE] */
}OPDEVSDK_POS_STRING_ST;


/**
* @brief �ַ����������
*/
typedef struct  _OPDEVSDK_POS_TEXT_STRING_ST_
{
    unsigned int                    strNum;             /*!< �ַ�������,��Χ[0,OPDEVSDK_POS_MAX_TEXT_LINE_NUM] */
    OPDEVSDK_POS_STRING_ST          *str;               /*!< �ַ�����Ϣ */
}OPDEVSDK_POS_TEXT_STRING_ST;


/**
* @brief Ԥ��pos�����Ϣ
*/
typedef struct  _OPDEVSDK_POS_TEXT_PARAM_ST_
{
    char                            *textBuf;           /*!< text���ݻ��� */
    unsigned int                    textBufLen;         /*!< text���泤�� */
    unsigned int                    *textLen;           /*!< text���ʵ�ʳ��� */
    unsigned int                    encWidth;           /*!< ������ */
    unsigned int                    encHeight;          /*!< ����߶� */
    unsigned int                    charSize;           /*!< �ַ���С */
    OPDEVSDK_POS_TEXT_STRING_ST     strList;            /*!< �ַ������� */
}OPDEVSDK_POS_TEXT_PARAM_ST;


/**
* @brief text��Ϣ(����Ԥ��pos���㷨pos�ȴ��)
*/
typedef struct _OPDEVSDK_POS_TEXT_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< ʱ������� */
    unsigned long long              timeStamp;          /*!< ʱ��� */
    unsigned int                    textLen;            /*!< text���ݳ��� */
    char                            *textBuf;           /*!< text���ݻ��� */
    unsigned int                    packId;             /*!< ���ID�������������ͣ���0��ʼ */
}OPDEVSDK_POS_TEXT_INFO_ST;


/**
* @brief ˽�����ݴ������ 
*/
typedef enum _OPDEVSDK_POS_DATA_PROC_TYPE_EN_
{
    OPDEVSDK_POS_DATA_PROC_TYPE_AFTER_ENC       = 0,        /*!< �������˽����Ϣ */
    OPDEVSDK_POS_DATA_PROC_TYPE_NONE            = 1,        /*!< �����˽����Ϣ */
    OPDEVSDK_POS_DATA_PROC_TYPE_MAX_NUM              = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_POS_DATA_PROC_TYPE_EN;


/** 
* @brief            ���Ŀ����Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pList           Ŀ�꼯������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procTarget(int chan,OPDEVSDK_POS_TARGET_LIST_INFO_ST *pTarget);


/** 
* @brief            ���������Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pList           ���򼯺�����
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procRule(int chan,OPDEVSDK_POS_RULE_LIST_INFO_ST *pRule);


/** 
* @brief            ���������Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pAlarm          ������������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlarm(int chan,OPDEVSDK_POS_ALERT_INFO_ST *pAlarm);


/** 
* @brief            Ԥ��pos��Ϣ��ʼ��
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pText           text��ʼ����Ϣ
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_initText(int chan,OPDEVSDK_POS_TEXT_PARAM_ST *pText);


/** 
* @brief            Ԥ��pos��Ϣ���
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pText           �ַ���Ϣ
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procText(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);

/** 
* @brief            �㷨Ԥ��pos��Ϣ���
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pText           �ַ���Ϣ
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlgText(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);


/** 
* @brief            ���������
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pText           �����
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procSearch(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);

/** 
* @brief            �������ݴ����������
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        streamId        ǰ��ͨ����Ӧ������ID
* @param[in]        procType        �����������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_setProcType(int chan,int streamId,OPDEVSDK_POS_DATA_PROC_TYPE_EN procType);


/** 
* @brief            ���Ŀ����Ϣ��֧����չ��Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pList           Ŀ�꼯������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procTarget_v2(int chan,OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST *pTarget);


/** 
* @brief            ���������Ϣ��֧����չ��Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pList           ���򼯺�����
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procRule_v2(int chan,OPDEVSDK_POS_RULE_LIST_INFO_V2_ST *pRule);


/** 
* @brief            ���������Ϣ��֧����չ��Ϣ
*
* @param[in]        chan            ǰ��ͨ����
* @param[in]        pAlarm          ������������
* @param[out]       none
*                   
* @return           0-�ɹ�,�������ش�����
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlarm_v2(int chan,OPDEVSDK_POS_ALERT_INFO_V2_ST *pAlarm);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_POS_H_ */


