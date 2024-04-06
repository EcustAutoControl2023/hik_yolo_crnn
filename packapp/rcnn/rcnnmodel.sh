#!/bin/sh

# cd /heop/package/yolomodel/G5/
cd /var/package/rcnnmodel/G5/
cp ./lib* /usr/lib/
./rcnnmodel ./config_rcnnmodel.txt FILE &
pwd
