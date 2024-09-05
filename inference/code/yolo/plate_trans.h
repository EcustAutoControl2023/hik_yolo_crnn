#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>

#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

// getTransForm函数
// 获取透视变换矩阵
// gray: 输入的图像
// dst_img: 输出的图像
// feature_point: 特征点
// debug: 是否开启debug模式
void getTransForm(cv::Mat gray, cv::Mat &dst_img, std::vector<cv::Point> feature_point, bool debug);

// getFeaturePoint函数
// 获取特征点
// gray: 输入的图像
// contour: 输入的轮廓
// feature_point: 输出的特征点
// delta: 偏移量
// debug: 是否开启debug模式
void getFeaturePoint(cv::Mat gray, std::vector<cv::Point> contour, std::vector<cv::Point> &feature_point, int delta, bool debug);

// getLicensePlateCandidates函数
// 获取车牌的候选区域
// gray: 输入的图像
// out_contours: 输出的轮廓
// keep: 保留的轮廓数
// debug: 是否开启debug模式
void getLicensePlateCandidates(cv::Mat &gray, std::vector<std::vector<cv::Point>> &out_contours, int keep, bool debug);
