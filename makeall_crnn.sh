#!/bin/sh
cur_dir=$(pwd)
echo $cur_dir
cd ./inference/build
make -f makefile_g5_crnn clean
make -f makefile_g5_crnn -j
cd ..
mv  crnnmodel ../packapp/crnn/crnnmodel
cp imagelist_crnn.txt ../packapp/crnn
cp config_crnnmodel.txt ../packapp/crnn
cp ./bin/myNet_ocr_P_G5_NPU0.bin ../packapp/crnn
mkdir -p ../packapp/crnn/data
cp ./data/crnn/* ../packapp/crnn/data
cp ./G5/lib/opdevsdk/*.so ../packapp/crnn
cd ../packapp/crnn
bash pack.sh
mv *.app ../../
pwd

