# curent path
cur_path=$(readlink -f "$(dirname "$0")")
custom_layer_path=$cur_path/../../../../../inference/custom_layer
chmod -R 755 $cur_path/../

# only *.c & *.cpp would be used
for file in `ls -Fp $custom_layer_path | grep "[c, cpp]$" | grep -v "custom"`
do
    # echo $file  # full filename with suffix
    # SUFFIX=${file##*.}  # suffix
    TARGET_NAME=${file%%.*}  # filename without suffix
    g++ -std=c++11 -o $cur_path/"$TARGET_NAME"_pc.so -shared -fPIC $cur_path/../../../../../inference/custom_layer/$file
    if [ $? -ne 0 ]; then
        exit 1
    fi
    mv $cur_path/"$TARGET_NAME"_pc.so $cur_path/../
    cp $cur_path/../../../../../inference/custom_layer/$TARGET_NAME.h $cur_path/../
done
