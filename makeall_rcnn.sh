#!/bin/sh
cur_dir=$(pwd)
echo $cur_dir
cd ./inference/build
make -f makefile_g5_rcnn clean
make -f makefile_g5_rcnn -j
cd ..
mv  rcnnmodel ../packapp/rcnn/rcnnmodel
cp imagelist_rcnn.txt ../packapp/rcnn
cp config_rcnnmodel.txt ../packapp/rcnn
cp ./bin/myNet_ocr_P_G5_NPU0.bin ../packapp/rcnn
mkdir -p ../packapp/rcnn/data
cp ./data/rcnn/* ../packapp/rcnn/data
cp ./G5/lib/opdevsdk/*.so ../packapp/rcnn
cd ../packapp/rcnn
bash pack.sh
mv *.app ../../
pwd

