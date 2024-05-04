#ifndef _PLATE_REC_H_
#define _PLATE_REC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 

#include "opdevsdk_sys.h"
#include "opdevsdk_jpegenc.h"
#include "opdevsdk_pos.h"
#include "opdevsdk_hikflow_lib.h"
#include "demo_comm.h"
// #define OPDEVSDK_S_OK       (0)     /*!< 基础服务库返回值，0代表成功 ，其他值请参考开发文档 */

#define plateName "#  京沪津渝冀晋蒙辽吉黑苏浙皖闽赣鲁豫鄂湘粤桂琼川贵云藏陕甘青宁新学警港澳挂使领民航危0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  G  H  J  K  L  M  N  P  Q  R  S  T  U  V  W  X  Y  Z  险品"

// argMax: 提取softmax函数结果中的最大值所在的索引
// input: softmax函数的结果
// length: softmax函数的结果长度
// return: 最大值所在的索引
int argMax(float *input, int length);

// softmax函数
// input: 输入的数组
// length: 数组长度
// return: 成功返回OPDEVSDK_S_OK
int softMax(float *input, int length);

// parsePlateName函数
// 将softmax函数的结果转换为车牌号
// input: softmax函数的结果的数组
// length: softmax函数的结果长度
// return: 车牌号char数组
char *parsePlateName(float **input, int length);

// parseRawData函数
// 将输入的数据转换为二维数组
// input: 输入的数据
// input_len: 输入数据的长度
// output: 输出的二维数组
// return: OPDEVSDK_S_OK if success, else OPDEVSDK_E_FAIL
int parseRawData(float *input, int input_len, float ***output);

#ifdef __cplusplus
}
#endif

#endif
