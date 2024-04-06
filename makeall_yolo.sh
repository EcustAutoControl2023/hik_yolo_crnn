#!/bin/sh
cur_dir=$(pwd)
echo $cur_dir
cd ./inference/build
make -f makefile_g5_yolo clean
make -f makefile_g5_yolo -j
cd ..
mv  yolomodel ../packapp/yolo/yolomodel
cp imagelist_yolo.txt ../packapp/yolo
cp config_yolomodel.txt ../packapp/yolo
cp ./bin/YoloV5_s6_P_G5_NPU0.bin ../packapp/yolo
mkdir -p ../packapp/yolo/data
cp ./data/yolo/* ../packapp/yolo/data
cp ./G5/lib/opdevsdk/*.so ../packapp/yolo
cd ../packapp/yolo
bash pack.sh
mv *.app ../../
pwd

