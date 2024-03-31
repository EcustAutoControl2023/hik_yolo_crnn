# -*- coding: utf-8 -*-
import sys
import ctypes
import caffe
import numpy as np
import copy

global aux_mem_size_global
## 加载动态库，其中包含forward和reshape
try:
    lib = ctypes.cdll.LoadLibrary("/heop/workspace/yolov5-tag6/model_optimizer/Sample/out/opc/YoloV5_s6_Region_pc.so")
except Exception as e:
    print(e)
    print("load lib failed")
    exit(-1)

# 引用forward和reshape函数
try:
    c_forward = lib.YoloV5_s6_Region_Forward
except Exception as e:
    print(e)
    print("undefined forward function: YoloV5_s6_Region_Forward")
    exit(-1)

try:
    c_reshape = lib.YoloV5_s6_Region_Reshape
except Exception as e:
    print(e)
    print("undefined resahpe function: YoloV5_s6_Region_Reshape")
    exit(-1)

# 自定义层 YoloV5_s6_Region 定义
class YoloV5_s6_Region(caffe.Layer):

    def setup(self, bottom, top):

        # 检查超参并赋值
        if not self.param_str:
            print("error: there is no hyperparam in the prototxt")
            exit(-1)
        hyper_str = eval(self.param_str)
        error_info = "error: hyper parameter (dest_param) does not exist in .prototxt"
        hyper_param_key = 'num_anchors'
        # 检查超参是否存在
        if hyper_param_key in hyper_str:
            self.num_anchors = hyper_str[hyper_param_key]
        else:
            error_info = error_info.replace("dest_param", hyper_param_key)
            print(error_info)
            sys.exit(-1)
        hyper_param_key = 'num_classes'
        # 检查超参是否存在
        if hyper_param_key in hyper_str:
            self.num_classes = hyper_str[hyper_param_key]
        else:
            error_info = error_info.replace("dest_param", hyper_param_key)
            print(error_info)
            sys.exit(-1)
        hyper_param_key = 'conf_thresh'
        # 检查超参是否存在
        if hyper_param_key in hyper_str:
            self.conf_thresh = hyper_str[hyper_param_key]
        else:
            error_info = error_info.replace("dest_param", hyper_param_key)
            print(error_info)
            sys.exit(-1)
        hyper_param_key = 'nms_thresh'
        # 检查超参是否存在
        if hyper_param_key in hyper_str:
            self.nms_thresh = hyper_str[hyper_param_key]
        else:
            error_info = error_info.replace("dest_param", hyper_param_key)
            print(error_info)
            sys.exit(-1)
    def forward(self, bottom, top):
        # 获取张量指针
        y1 = bottom[0].data.ctypes.data_as(ctypes.c_void_p)
        y2 = bottom[1].data.ctypes.data_as(ctypes.c_void_p)
        y3 = bottom[2].data.ctypes.data_as(ctypes.c_void_p)
        y4 = bottom[3].data.ctypes.data_as(ctypes.c_void_p)
        d0 = copy.deepcopy(bottom[0].data)
        y1_temp = d0.ctypes.data_as(ctypes.c_void_p)
        d1 = copy.deepcopy(bottom[1].data)
        y2_temp = d1.ctypes.data_as(ctypes.c_void_p)
        d2 = copy.deepcopy(bottom[2].data)
        y3_temp = d2.ctypes.data_as(ctypes.c_void_p)
        d3 = copy.deepcopy(bottom[3].data)
        y4_temp = d3.ctypes.data_as(ctypes.c_void_p)
        output = top[0].data.ctypes.data_as(ctypes.c_void_p)

        # 创建输出array并转换为指针以保存输出blob的shape
        outshape_temp = np.zeros(4 , dtype=np.int32)
        outshape = outshape_temp.ctypes.data_as(ctypes.c_void_p)

        in1_n = bottom[0].data.shape[shape_enum.in_n]
        in1_c = bottom[0].data.shape[shape_enum.in_c]
        in1_h = bottom[0].data.shape[shape_enum.in_h]
        in1_w = bottom[0].data.shape[shape_enum.in_w]
        in2_n = bottom[1].data.shape[shape_enum.in_n]
        in2_c = bottom[1].data.shape[shape_enum.in_c]
        in2_h = bottom[1].data.shape[shape_enum.in_h]
        in2_w = bottom[1].data.shape[shape_enum.in_w]
        in3_n = bottom[2].data.shape[shape_enum.in_n]
        in3_c = bottom[2].data.shape[shape_enum.in_c]
        in3_h = bottom[2].data.shape[shape_enum.in_h]
        in3_w = bottom[2].data.shape[shape_enum.in_w]
        in4_n = bottom[3].data.shape[shape_enum.in_n]
        in4_c = bottom[3].data.shape[shape_enum.in_c]
        in4_h = bottom[3].data.shape[shape_enum.in_h]
        in4_w = bottom[3].data.shape[shape_enum.in_w]
        num_anchors = self.num_anchors
        num_classes = self.num_classes
        conf_thresh = self.conf_thresh
        nms_thresh = self.nms_thresh
       
        global aux_mem_size_global
        # 创建一个占位空间给reshap函数，实际对python层没有作用
        # print("pc froward aux mem size: ", aux_mem_size_global)
        __auxi_mem_addr_temp = np.zeros(aux_mem_size_global, dtype=np.int8)
        __auxi_mem_addr__ = __auxi_mem_addr_temp.ctypes.data_as(ctypes.c_void_p)
 

        # 声明参数类型
        c_forward.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_float, ctypes.c_float, ctypes.c_void_p]
        # 调用C库
        ret = c_forward(y1_temp, in1_n, in1_c, in1_h, in1_w, y2_temp, in2_n, in2_c, in2_h, in2_w, y3_temp, in3_n, in3_c, in3_h, in3_w, y4_temp, in4_n, in4_c, in4_h, in4_w, output, outshape, num_anchors, num_classes, conf_thresh, nms_thresh, __auxi_mem_addr__)      

        if 0 != ret:
            print("Error: error in .so forward function.")
            sys.exit(-1)

        # arry转为list 若某个shape为0，则使其为1
        outshape_temp = outshape_temp.tolist()
        for i in range(len(outshape_temp)):
            if 0 == outshape_temp[i]:
                outshape_temp[i] = 1

        # 进行top的reshape
            top[0].reshape(outshape_temp[0],
                           outshape_temp[1],
                           outshape_temp[2],
                           outshape_temp[3])

    def backward(self, bottom, top):
        # 自动生成工具不生成此代码
        pass

    def reshape(self, bottom, top):
        # 创建输出array并转换为指针以保存输出blob的shape
        outshape_temp = np.zeros(4, dtype=np.int32)
        outshape = outshape_temp.ctypes.data_as(ctypes.c_void_p)
        # 创建一个占位空间给reshap函数，实际对python层没有作用
        assist_mem_size_temp = np.zeros(1, dtype=np.int32)
        assist_mem_size = assist_mem_size_temp.ctypes.data_as(ctypes.c_void_p)
        in1_n = bottom[0].data.shape[shape_enum.in_n]
        in1_c = bottom[0].data.shape[shape_enum.in_c]
        in1_h = bottom[0].data.shape[shape_enum.in_h]
        in1_w = bottom[0].data.shape[shape_enum.in_w]
        in2_n = bottom[1].data.shape[shape_enum.in_n]
        in2_c = bottom[1].data.shape[shape_enum.in_c]
        in2_h = bottom[1].data.shape[shape_enum.in_h]
        in2_w = bottom[1].data.shape[shape_enum.in_w]
        in3_n = bottom[2].data.shape[shape_enum.in_n]
        in3_c = bottom[2].data.shape[shape_enum.in_c]
        in3_h = bottom[2].data.shape[shape_enum.in_h]
        in3_w = bottom[2].data.shape[shape_enum.in_w]
        in4_n = bottom[3].data.shape[shape_enum.in_n]
        in4_c = bottom[3].data.shape[shape_enum.in_c]
        in4_h = bottom[3].data.shape[shape_enum.in_h]
        in4_w = bottom[3].data.shape[shape_enum.in_w]
        num_anchors = self.num_anchors
        # 声明参数类型
        c_reshape.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
        # 调用C库
        ret = c_reshape(in1_n, in1_c, in1_h, in1_w, in2_n, in2_c, in2_h, in2_w, in3_n, in3_c, in3_h, in3_w, in4_n, in4_c, in4_h, in4_w, num_anchors, assist_mem_size, outshape)

        if 0 != ret:
            print("Error: reshape error")
            sys.exit(-1)
        global aux_mem_size_global
        # 创建一个占位空间给reshap函数，实际对python层没有作用
        aux_mem_size_global = assist_mem_size_temp[0]
        # print("reshape aux mem size: ", aux_mem_size_global)
     
        # arry转为list 若某个shape为0，则使其为1
        outshape_temp = outshape_temp.tolist()
        for i in range(len(outshape_temp)):
            if 0 == outshape_temp[i]:
                outshape_temp[i] = 1

        # 进行top的reshape
            top[0].reshape(outshape_temp[0],
                           outshape_temp[1],
                           outshape_temp[2],
                           outshape_temp[3])

class shape_enum():
    in_n = 0
    in_c = 1
    in_h = 2
    in_w = 3