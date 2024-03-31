/***************************************************************************
* note 2012-2029 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_pos.h
* @brief        基础库POS(play on screen)模块
* @note         只适用于将信息传入码流情况
*
* @date         2019/5/16
* @version      1.0.0
* @note         增加luma/expose打包接口
*
* @date         2019/6/1
* @version      2.0.0
* @note         同步海康内部接口
*
* @date         2020/2/19
* @version      2.0.1
* @note         将目标数组改为指针(target list、rule list、luma)，便于扩展目标数量
*
* @date         2020/3/20
* @version      2.0.2
* @note         1.增加procType打包类型接口,
*               2.将luma、expose接口移至ISP模块
*****************************************************************************/

#ifndef _OPDEVSDK_POS_H_
#define _OPDEVSDK_POS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opdevsdk_mediadrv_comm.h"


#define OPDEVSDK_POS_MAX_RULE_NUM           (64)        /*!< 规则打包最大数量 */
#define OPDEVSDK_POS_MAX_TARGET_NUM         (512)       /*!< 目标打包最大数量 */
#define OPDEVSDK_POS_MAX_ALERT_NUM          (64)        /*!< 报警打包最大数量 */
#define OPDEVSDK_POS_MAX_RULE_VERTEX_NUM    (10)        /*!< 规则区域最多顶点数量 */
#define OPDEVSDK_POS_MAX_TARGET_VERTEX_NUM  (10)        /*!< 目标区域最多顶点数量 */
#define OPDEVSDK_POS_MAX_TEXT_LINE_NUM      (32)        /*!< 预览pos叠加最大行数 */
#define OPDEVSDK_POS_MAX_CHAR_PER_LINE      (44)        /*!< 预览pos每行叠加字符最大数量 */

#define OPDEVSDK_POS_PACKAGE_ID_BASE        (0)         /*!< 打包ID,可用于区分同时间戳不同智能的私有信息 */

#define OPDEVSDK_POS_MAX_PRIV_DATA_LEN      (32)        /*!< 最大私有数据长度 */

/*!< 生成RGB颜色,type定义参考OPDEVSDK_POS_RGB_COLOR_TYPE_EN */
#define OPDEVSDK_POS_MAKE_RGB_COLOR(type,b,g,r)     (((type&0xff)<<24)|((b&0xff)<<16)|((g&0xff)<<8)|(r&0xff))

/**
* @brief 自定义目标、规则、轨迹颜色类别
*/
typedef enum _OPDEVSDK_POS_RGB_COLOR_TYPE_EN_
{
    OPDEVSDK_POS_RGB_COLOR_TYPE_NONE                = 0,        /*!< 未定义,使用默认颜色 */
    OPDEVSDK_POS_RGB_COLOR_TYPE_RULE                = 1,        /*!< 规则框自定义颜色类别 */
    OPDEVSDK_POS_RGB_COLOR_TYPE_TARGET              = 2,        /*!< 目标框自定义颜色类别 */
    OPDEVSDK_POS_RGB_COLOR_TYPE_HUMAN_TRACE         = 3,        /*!< 人体轨迹自定义颜色类别 */
    OPDEVSDK_POS_RGB_COLOR_TYPE_VEHICLE_TRACE       = 4,        /*!< 机动车轨迹自定义颜色类别 */
    OPDEVSDK_POS_RGB_COLOR_TYPE_MAX                 = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
}OPDEVSDK_POS_RGB_COLOR_TYPE_EN;

/**
* @brief 单坐标点描述
*/
typedef struct _OPDEVSDK_POS_POINT_ST_
{
    float       x;      /*!< X轴归一化坐标,范围[0,1] */
    float       y;      /*!< Y轴归一化坐标,范围[0,1] */
}OPDEVSDK_POS_POINT_ST;


/**
* @brief 矩形目标区域描述
*/
typedef struct _OPDEVSDK_POS_RECT_ST_
{
    float       x;          /*!< X轴归一化坐标,范围[0,1] */
    float       y;          /*!< Y轴归一化坐标,范围[0,1] */
    float       width;      /*!< X轴归一化宽度,范围(0,1] */
    float       height;     /*!< X轴归一化高度,范围(0,1] */
}OPDEVSDK_POS_RECT_ST;


/**
* @brief 目标区域描述
*/
typedef struct _OPDEVSDK_POS_REGION_ST_
{
    unsigned int                pointNum;                                       /*!< 区域对应坐标点数量 */ 
    OPDEVSDK_POS_POINT_ST       point[OPDEVSDK_POS_MAX_TARGET_VERTEX_NUM];      /*!< 区域所有坐标点描述 */
}OPDEVSDK_POS_REGION_ST;


/**
* @brief 目标区域类型
*/
typedef enum _OPDEVSDK_POS_REGION_TYPE_EN_
{
    OPDEVSDK_POS_REGION_NONE                = 0,            /*!< 未定义类型 */
    OPDEVSDK_POS_REGION_POLYLINE            = 1,            /*!< 折线类型 */
    OPDEVSDK_POS_REGION_POLYGON             = 2,            /*!< 多边形 */
    OPDEVSDK_POS_REGION_RECT                = 3,            /*!< 矩形 */
    OPDEVSDK_POS_REGION_MAX                 = OPDEVSDK_MEDIADRV_ENUM_MAXI 
}OPDEVSDK_POS_REGION_TYPE_EN;


/**
* @brief 目标区域描述
*/
typedef struct _OPDEVSDK_POS_REGION_V2_ST_
{
    unsigned int    regionType;                             /*!< 区域类型，参考OPDEVSDK_POS_REGION_TYPE_EN */
	char            res[12];                                /*!< 保留字段 */
	union
	{
		unsigned char		            size[84];           /*!< 保留字段，确认联合体大小 */
		OPDEVSDK_POS_REGION_ST          polygon;            /*!< 多边形、直线、折线目标参数 */
		OPDEVSDK_POS_RECT_ST            rect;               /*!< 矩形目标参数 */
	};
}OPDEVSDK_POS_REGION_V2_ST;


/**
* @brief 多边形显示属性(颜色、宽度、样式等)
*/
typedef enum _OPDEVSDK_POS_POLYGON_ATTRI_EN_
{
    OPDEVSDK_POS_POLYGON_ATTRI_NORMAL               = 0,        /*!< 普通智能类型,绿色目标框,蓝色规则框,报警框红色 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBG_ONE_HALF      = 1,        /*!< 规则框,普通绿色1/2 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBR_ONE_HALF      = 2,        /*!< 目标框,加粗红色1/2 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBG_TWO_THIRD     = 3,        /*!< 规则框,普通绿色2/3 */
    OPDEVSDK_POS_POLYGON_ATTRI_LINEBR_TWO_THIRD     = 4,        /*!< 目标框,加粗红色2/3 */
    OPDEVSDK_POS_POLYGON_ATTRI_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
} OPDEVSDK_POS_POLYGON_ATTRI_EN;


/**
* @brief 时间戳类型，用于区分RTSP码流还是YUV数据携带的时间戳
*/
typedef enum _OPDEVSDK_POS_TIME_TYPE_EN_
{
    OPDEVSDK_POS_TIME_TYPE_1K                   = 0,        /*!< 时间戳,单位:1KB，或认为毫秒 */
    OPDEVSDK_POS_TIME_TYPE_90K                  = 1,        /*!< 时间戳,单位:90KB */
    OPDEVSDK_POS_TIME_TYPE_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
} OPDEVSDK_POS_TIME_TYPE_EN;


/**
* @brief 单目标描述
*/
typedef struct _OPDEVSDK_POS_TARGET_ST_
{
    unsigned int                id;             /*!< 目标ID */
    unsigned int                trace_time;     /*!< 轨迹显示时间,目标框为轨迹类型时有效,范围(0,60),默认30,单位秒 */
    unsigned int                color;          /*!< 颜色信息,RGB格式 ，这里color生效时，OPDEVSDK_POS_POLYGON_ATTRI_EN里的颜色属性失效 */
    OPDEVSDK_POS_REGION_ST      region;         /*!< 目标区域信息 */	
    int                         res[4];         /*!< 保留字段 */
}OPDEVSDK_POS_TARGET_ST;


/**
* @brief 单目标描述
*/
typedef struct _OPDEVSDK_POS_TARGET_V2_ST_
{
    unsigned int                    id;             /*!< 目标ID */
    OPDEVSDK_POS_REGION_V2_ST       region;         /*!< 目标区域信息 */	
    int                             res[4];         /*!< 保留字段 */
}OPDEVSDK_POS_TARGET_V2_ST;


/**
* @brief 目标链表描述
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_ST_
{
    unsigned int                tgtNum;         /*!< 目标数量,范围[0,OPDEVSDK_POS_MAX_TARGET_NUM] */
    OPDEVSDK_POS_TARGET_ST      *pTgt;          /*!< 所有目标描述 */
}OPDEVSDK_POS_TARGET_LIST_ST;


/**
* @brief 具有相同属性(时间戳、颜色、宽度、样式等)的目标信息集合
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< 时间戳类型，通过接入码流方式获取待分析数据的，建议采用90K时间戳 */
    unsigned long long                  timeStamp;      /*!< 时间戳 */
    unsigned int                        packId;         /*!< 打包ID，区分智能类型，从0开始 */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< 目标框显示属性 */
    OPDEVSDK_POS_TARGET_LIST_ST         tgtList;        /*!< 目标链表 */
    int                                 res[8];         /*!< 保留字段 */    
}OPDEVSDK_POS_TARGET_LIST_INFO_ST;


/**
* @brief 颜色信息
*/
typedef struct _OPDEVSDK_POS_EX_INFO_COLOR_ST_
{
    unsigned char red;          /*!< 红色分类 */
    unsigned char green;        /*!< 绿色分量 */
    unsigned char blue;         /*!< 蓝色分量 */
    unsigned char alpha;        /*!< alpha色彩空间，暂未使用 */
}OPDEVSDK_POS_EX_INFO_COLOR_ST;

/**
* @brief 自定义轨迹类别
*/
typedef enum _OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN_
{
    OPDEVSDK_POS_EX_TRACE_TYPE_NONE                 = 0,        /*!< 未定义 */
    OPDEVSDK_POS_EX_TRACE_TYPE_HUMAN_TRACE          = 1,        /*!< 人体轨迹类别 */
    OPDEVSDK_POS_EX_TRACE_TYPE_VEHICLE_TRACE        = 2,        /*!< 机动车轨迹类别 */
    OPDEVSDK_POS_EX_TRACE_TYPE_MAX                  = OPDEVSDK_MEDIADRV_ENUM_MAXI                           
}OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN;

/**
* @brief 颜色轨迹信息
*/
typedef struct _OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST_
{
    OPDEVSDK_POS_EX_INFO_COLOR_ST           color;          /*!< 颜色信息 */
    unsigned char                           lineTime;       /*!< 轨迹持续时间，范围(0,60)，单位:秒 */
    unsigned char                           reseverd[3];    /*!< 保留字段 */
    OPDEVSDK_POS_EX_INFO_TRACE_TYPE_EN      targetType;     /*!< 目标轨迹类型，1-人轨迹，2-车轨迹 */
}OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST;


/**
* @brief 私有扩展信息类型
*/
typedef enum _OPDEVSDK_POS_EX_INFO_TYPE_EN_
{
    OPDEVSDK_POS_EX_INFO_NONE               = 0,            /*!< 无扩展信息 */
    OPDEVSDK_POS_EX_INFO_COLOR              = 1,            /*!< 扩展颜色信息(扩展数据类型OPDEVSDK_POS_EX_INFO_COLOR_ST) */
    OPDEVSDK_POS_EX_INFO_COLOR_LINE         = 2,            /*!< 颜色轨迹信息(扩展数据类型OPDEVSDK_POS_EX_INFO_COLOR_TRACE_ST) */
    OPDEVSDK_POS_EX_INFO_SPEED_COORD        = 3,            /*!< 速度坐标信息(暂不支持) */
    OPDEVSDK_POS_EX_INFO_COORD_S            = 4,            /*!< 经纬度信息（暂不支持） */
    OPDEVSDK_POS_EX_INFO_EAGLE_R            = 5,            /*!< 鹰眼跟踪红色四角框(无扩展数据) */
    OPDEVSDK_POS_EX_INFO_MAX                = OPDEVSDK_MEDIADRV_ENUM_MAXI 
}OPDEVSDK_POS_EX_INFO_TYPE_EN;


/**
* @brief 私有扩展信息描述
*/
typedef struct _OPDEVSDK_POS_EX_INFO_ST_
{
    unsigned char                       exType;                                     /*!< 扩展信息类型参考OPDEVSDK_POS_EX_INFO_TYPE_EN */
    unsigned char                       reseverd[14];                               /*!< 保留字段 */
    unsigned char                       exLen;                                      /*!< 扩展信息长度 */
    unsigned char                       exData[OPDEVSDK_POS_MAX_PRIV_DATA_LEN];     /*!< 扩展数据，数据类型与exType相关 */
}OPDEVSDK_POS_EX_INFO_ST;


/**
* @brief 目标信息描述，支持扩展信息
*/
typedef struct _OPDEVSDK_POS_TARGET_EX_ST_
{
    OPDEVSDK_POS_TARGET_V2_ST           tgt;            /*!< 目标信息 */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< 扩展信息 */
}OPDEVSDK_POS_TARGET_EX_ST;


/**
* @brief 目标链表描述
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_V2_ST_
{
    unsigned int                    tgtNum;         /*!< 目标数量,范围[0,OPDEVSDK_POS_MAX_TARGET_NUM] */
    OPDEVSDK_POS_TARGET_EX_ST       *pTgt;          /*!< 所有目标描述 */
}OPDEVSDK_POS_TARGET_LIST_V2_ST;


/**
* @brief 具有相同属性(时间戳等)的目标信息集合
*/
typedef struct _OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< 时间戳类型，通过接入码流方式获取待分析数据的，建议采用90K时间戳 */
    unsigned long long                  timeStamp;      /*!< 时间戳 */
    unsigned int                        packId;         /*!< 打包ID，区分智能类型，从0开始 */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< 目标框显示属性 */
    OPDEVSDK_POS_TARGET_LIST_V2_ST      tgtList;        /*!< 目标链表 */
    int                                 res[8];         /*!< 保留字段 */    
}OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST;


/**
* @brief 规则类型
*/
typedef enum _OPDEVSDK_POS_RULE_TYPE_EN_
{
    OPDEVSDK_POS_RULE_TYPE_LINE         = 0,        /*!< 线规则 ，应用场景如跨线等 */
    OPDEVSDK_POS_RULE_TYPE_REGION       = 1,        /*!< 区域规则，应用场景如区域入侵，离开区域等 */
    OPDEVSDK_POS_RULE_TYPE_MAX_NUM      = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_POS_RULE_TYPE_EN;


/**
* @brief 规则区域描述
*/
typedef struct _OPDEVSDK_POS_RULE_POLYGON_ST_
{
    unsigned int                pointNum;                                   /*!< 区域坐标点数量 */
    OPDEVSDK_POS_POINT_ST       point[OPDEVSDK_POS_MAX_RULE_VERTEX_NUM];    /*!< 坐标点信息 */
}OPDEVSDK_POS_RULE_POLYGON_ST;


/**
* @brief 单规则描述
*/
typedef struct _OPDEVSDK_POS_RULE_ST_
{
    unsigned int                        id;             /*!< 规则ID */
    int                                 enable;         /*!< 规则使能,TRUE表示使能,FALSE表示非使能 */
    OPDEVSDK_POS_RULE_TYPE_EN           ruleType;       /*!< 规则类型 */
    OPDEVSDK_POS_RULE_POLYGON_ST        polygon;        /*!< 规则参数描述 */
    unsigned int                        color;          /*!< 颜色信息,RGB格式,无扩展信息(exInfo)时有效,参考OPDEVSDK_POS_MAKE_RGB_COLOR */
    int                                 reserved[3];    /*!< 保留字段 */
}OPDEVSDK_POS_RULE_ST;


/**
* @brief 规则链表描述
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_ST_
{
    unsigned int                        ruleNum;        /*!< 规则数量,范围[0,OPDEVSDK_POS_MAX_RULE_NUM] */
    OPDEVSDK_POS_RULE_ST                *pRule;         /*!< 规则信息 */
}OPDEVSDK_POS_RULE_LIST_ST;


/**
* @brief 具有相同属性(时间戳、颜色、宽度、样式等)的规则信息集合
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< 时间戳类型 */
    unsigned long long                  timeStamp;      /*!< 时间戳 */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< 规则框显示属性 */
    OPDEVSDK_POS_RULE_LIST_ST           ruleList;       /*!< 规则链表 */
    unsigned int                        packId;         /*!< 打包ID，区分智能类型，从0开始 */
}OPDEVSDK_POS_RULE_LIST_INFO_ST;


/**
* @brief 规则信息描述，支持扩展信息
*/
typedef struct _OPDEVSDK_POS_RULE_V2_ST_
{
    OPDEVSDK_POS_RULE_ST                rule;           /*!< 规则信息 */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< 扩展信息 */
}OPDEVSDK_POS_RULE_V2_ST;


/**
* @brief 规则链表描述
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_V2_ST_
{
    unsigned int                        ruleNum;        /*!< 规则数量,范围[0,OPDEVSDK_POS_MAX_RULE_NUM] */
    OPDEVSDK_POS_RULE_V2_ST             *pRule;         /*!< 规则信息 */
}OPDEVSDK_POS_RULE_LIST_V2_ST;


/**
* @brief 具有相同属性(时间戳等)的规则信息集合
*/
typedef struct _OPDEVSDK_POS_RULE_LIST_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN           timeType;       /*!< 时间戳类型 */
    unsigned long long                  timeStamp;      /*!< 时间戳 */
    OPDEVSDK_POS_POLYGON_ATTRI_EN       attribute;      /*!< 规则框显示属性 */
    OPDEVSDK_POS_RULE_LIST_V2_ST        ruleList;       /*!< 规则链表 */
    unsigned int                        packId;         /*!< 打包ID，区分智能类型，从0开始 */
}OPDEVSDK_POS_RULE_LIST_INFO_V2_ST;


/**
* @brief 报警信息结合
*/
typedef struct  _OPDEVSDK_POS_ALERT_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< 时间戳类型 */
    unsigned long long              timeStamp;          /*!< 时间戳 */
    unsigned char                   alert;              /*!< 报警标志,0表示无报警,1表示报警 */
    unsigned char                   viewState;          /*!< 场景状态,0表示正常,1表示场景变更,2表示相机遮挡 */
    unsigned char                   res[6];             /*!< 保留字段 */
    OPDEVSDK_POS_RULE_ST            ruleInfo;           /*!< 报警规则信息 */
    OPDEVSDK_POS_TARGET_ST          target;             /*!< 报警目标信息 */ 
    unsigned int                    packId;             /*!< 打包ID，区分智能类型，从0开始 */
}OPDEVSDK_POS_ALERT_INFO_ST;

/**
* @brief 报警目标、规则描述，支持扩展信息
*/
typedef struct _OPDEVSDK_POS_ALERT_V2_ST_
{
    OPDEVSDK_POS_RULE_ST                rule;           /*!< 规则信息 */
    OPDEVSDK_POS_TARGET_V2_ST           tgt;            /*!< 目标信息 */
    OPDEVSDK_POS_EX_INFO_ST             exInfo;         /*!< 扩展信息 */
}OPDEVSDK_POS_ALERT_V2_ST;

/**
* @brief 报警目标、规则描述，支持扩展信息
*/
typedef struct _OPDEVSDK_POS_ALERT_LIST_V2_ST_
{
    unsigned int                        alertNum;       /*!< 报警数量,范围[0,OPDEVSDK_POS_MAX_ALERT_NUM] */
    OPDEVSDK_POS_ALERT_V2_ST            *pAlert;        /*!< 报警信息 */
}OPDEVSDK_POS_ALERT_LIST_V2_ST;

/**
* @brief 报警信息结合
*/
typedef struct  _OPDEVSDK_POS_ALERT_INFO_V2_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< 时间戳类型 */
    unsigned long long              timeStamp;          /*!< 时间戳 */
    unsigned char                   alert;              /*!< 报警标志,0表示无报警,1表示报警 */
    unsigned char                   viewState;          /*!< 场景状态,0表示正常,1表示场景变更,2表示相机遮挡 */
    unsigned char                   res[6];             /*!< 保留字段 */
    OPDEVSDK_POS_ALERT_LIST_V2_ST   alertList;          /*!< 报警信息 */ 
    unsigned int                    packId;             /*!< 打包ID，区分智能类型，从0开始 */
}OPDEVSDK_POS_ALERT_INFO_V2_ST;


/**
* @brief 字符串信息
*/
typedef struct  _OPDEVSDK_POS_STRING_ST_
{
    OPDEVSDK_POS_POINT_ST           point;              /*!< pos坐标 */
    char                            *str;               /*!< 字符串 */
    unsigned int                    strLen;             /*!< 字符串长度,范围[0,OPDEVSDK_POS_MAX_CHAR_PER_LINE] */
}OPDEVSDK_POS_STRING_ST;


/**
* @brief 字符串链表参数
*/
typedef struct  _OPDEVSDK_POS_TEXT_STRING_ST_
{
    unsigned int                    strNum;             /*!< 字符串数量,范围[0,OPDEVSDK_POS_MAX_TEXT_LINE_NUM] */
    OPDEVSDK_POS_STRING_ST          *str;               /*!< 字符串信息 */
}OPDEVSDK_POS_TEXT_STRING_ST;


/**
* @brief 预览pos打包信息
*/
typedef struct  _OPDEVSDK_POS_TEXT_PARAM_ST_
{
    char                            *textBuf;           /*!< text数据缓存 */
    unsigned int                    textBufLen;         /*!< text缓存长度 */
    unsigned int                    *textLen;           /*!< text打包实际长度 */
    unsigned int                    encWidth;           /*!< 编码宽度 */
    unsigned int                    encHeight;          /*!< 编码高度 */
    unsigned int                    charSize;           /*!< 字符大小 */
    OPDEVSDK_POS_TEXT_STRING_ST     strList;            /*!< 字符串链表 */
}OPDEVSDK_POS_TEXT_PARAM_ST;


/**
* @brief text信息(用于预览pos和算法pos等打包)
*/
typedef struct _OPDEVSDK_POS_TEXT_INFO_ST_
{
    OPDEVSDK_POS_TIME_TYPE_EN       timeType;           /*!< 时间戳类型 */
    unsigned long long              timeStamp;          /*!< 时间戳 */
    unsigned int                    textLen;            /*!< text数据长度 */
    char                            *textBuf;           /*!< text数据缓存 */
    unsigned int                    packId;             /*!< 打包ID，区分智能类型，从0开始 */
}OPDEVSDK_POS_TEXT_INFO_ST;


/**
* @brief 私有数据打包类型 
*/
typedef enum _OPDEVSDK_POS_DATA_PROC_TYPE_EN_
{
    OPDEVSDK_POS_DATA_PROC_TYPE_AFTER_ENC       = 0,        /*!< 编码后打包私有信息 */
    OPDEVSDK_POS_DATA_PROC_TYPE_NONE            = 1,        /*!< 不打包私有信息 */
    OPDEVSDK_POS_DATA_PROC_TYPE_MAX_NUM              = OPDEVSDK_MEDIADRV_ENUM_MAXI,
}OPDEVSDK_POS_DATA_PROC_TYPE_EN;


/** 
* @brief            打包目标信息
*
* @param[in]        chan            前端通道号
* @param[in]        pList           目标集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procTarget(int chan,OPDEVSDK_POS_TARGET_LIST_INFO_ST *pTarget);


/** 
* @brief            打包规则信息
*
* @param[in]        chan            前端通道号
* @param[in]        pList           规则集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procRule(int chan,OPDEVSDK_POS_RULE_LIST_INFO_ST *pRule);


/** 
* @brief            打包报警信息
*
* @param[in]        chan            前端通道号
* @param[in]        pAlarm          报警集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlarm(int chan,OPDEVSDK_POS_ALERT_INFO_ST *pAlarm);


/** 
* @brief            预览pos信息初始化
*
* @param[in]        chan            前端通道号
* @param[in]        pText           text初始化信息
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_initText(int chan,OPDEVSDK_POS_TEXT_PARAM_ST *pText);


/** 
* @brief            预览pos信息打包
*
* @param[in]        chan            前端通道号
* @param[in]        pText           字符信息
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procText(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);

/** 
* @brief            算法预览pos信息打包
*
* @param[in]        chan            前端通道号
* @param[in]        pText           字符信息
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlgText(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);


/** 
* @brief            后检索处理
*
* @param[in]        chan            前端通道号
* @param[in]        pText           后检索
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procSearch(int chan,OPDEVSDK_POS_TEXT_INFO_ST *pText);

/** 
* @brief            设置数据打包码流类型
*
* @param[in]        chan            前端通道号
* @param[in]        streamId        前端通道对应的码流ID
* @param[in]        procType        打包处理类型
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_setProcType(int chan,int streamId,OPDEVSDK_POS_DATA_PROC_TYPE_EN procType);


/** 
* @brief            打包目标信息，支持扩展信息
*
* @param[in]        chan            前端通道号
* @param[in]        pList           目标集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procTarget_v2(int chan,OPDEVSDK_POS_TARGET_LIST_INFO_V2_ST *pTarget);


/** 
* @brief            打包规则信息，支持扩展信息
*
* @param[in]        chan            前端通道号
* @param[in]        pList           规则集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procRule_v2(int chan,OPDEVSDK_POS_RULE_LIST_INFO_V2_ST *pRule);


/** 
* @brief            打包报警信息，支持扩展信息
*
* @param[in]        chan            前端通道号
* @param[in]        pAlarm          报警集合描述
* @param[out]       none
*                   
* @return           0-成功,其他返回错误码
*/
OPDEVSDK_EXPORT int opdevsdk_pos_procAlarm_v2(int chan,OPDEVSDK_POS_ALERT_INFO_V2_ST *pAlarm);


#ifdef __cplusplus
}
#endif


#endif /* _OPDEVSDK_POS_H_ */


