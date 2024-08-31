#include <string>

#include <cstdlib>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>

#include "demo_alg.h"


void letterbox(
    cv::Mat* img, 
    unsigned int width=416,
    unsigned int height=416,
    unsigned int bgColor=0,
    unsigned int _auto=1,
    unsigned int scaleFill=0,
    unsigned int scaleup=1,
    unsigned int stride=1000
);

void _resize_img(
    cv::Mat* img,
    char* img_path,
    bool size_change,
    CONFIG_DATA_T* config_data
);

void mat_to_bgr_planner(
    cv::Mat* img,
    const char* img_path,
    const char* bgr_path,
    bool size_change,
    bool bgr_planner,
    CONFIG_DATA_T* config_data
);

// TODO:  待实现
void jpg2nv12(
    cv::Mat* img,
    const char* img_path,
    const char* nv12_path,
    bool size_change,
    CONFIG_DATA_T* config_data
);

void jpg2nv21(
    cv::Mat* img,
    const char* img_path,
    const char* nv21_path,
    bool size_change,
    CONFIG_DATA_T* config_data
);

void convert_images(
    CONFIG_DATA_T* config_data
);
