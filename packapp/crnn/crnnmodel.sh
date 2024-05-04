#!/bin/sh

# cd /heop/package/yolomodel/G5/
cd /var/package/crnnmodel/G5/
cp ./lib* /usr/lib/
./crnnmodel ./config_crnnmodel.txt FILE &
pwd
