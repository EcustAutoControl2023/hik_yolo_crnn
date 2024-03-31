#!/bin/sh
cur_dir=$(pwd)
echo $cur_dir
cd ./inference/build
make -f makefile_g5 clean
make -f makefile_g5 -j
cd ..
mv  yolomodel ../packapp/yolomodel
cp *.txt ../packapp
cp ./bin/*.bin ../packapp
# cp ./data/* ../packapp
cp ./G5/lib/opdevsdk/*.so ../packapp
cd ../packapp
bash pack.sh
mv *.app ../
pwd

