#!/bin/sh

# cd /heop/package/yolomodel/G5/
cd /var/package/yolomodel/G5/
cp ./lib* /usr/lib/
./yolomodel ./config_yolomodel.txt YUV &
pwd