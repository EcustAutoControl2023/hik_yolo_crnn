/***************************************************************************
* note 2012-2019 HangZhou Hikvision Digital Technology Co., Ltd. All Right Reserved.
*
* @file         opdevsdk_she.h
* @brief        the interface of scheduler
*
*
* @date         2019/2/18
* @version      1.0.0
* @note         add init/deinit/load bin interface
*
* @date         2020/5/23
* @version      1.0.0
* @note         delete load bin/unload bin intefaces

*****************************************************************************/


#ifndef _OPDEVSDK_SCHE_H_
#define _OPDEVSDK_SCHE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OPDEVSDK_EXPORT
#define OPDEVSDK_EXPORT __attribute__((visibility("default")))
#endif

/**
* @brief Correct return value
*/
#define OPDEVSDK_SCHE_S_OK                     			    (0)					/*!<  return sucess */


#define OPDEVSDK_SCHE_VERSION_CHAR                          (128)


/** 
* @brief            Initializes the scheduler
*
* @param[in]        none
* @param[out]       none
*                   
* @return           0 if successful, otherwise an error number returned
*/
OPDEVSDK_EXPORT int opdevsdk_sche_init();


/** 
* @brief            Deinitializes the scheduler
*
* @param[in]        none
* @param[out]       none
*                   
* @return           0 if successful, otherwise an error number returned
*/
OPDEVSDK_EXPORT int opdevsdk_sche_deinit(void);


#ifdef __cplusplus
}
#endif

#endif //_OPDEVSDK_SCHE_


