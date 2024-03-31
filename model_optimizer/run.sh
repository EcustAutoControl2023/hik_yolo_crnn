pwd_path=$(readlink -f "$(dirname "$0")")
status=$?
# third project environment configuration
export PYTHONPATH=$pwd_path/SourceProject/yolov5-6:$PYTHONPATH

echo "[demo] start running demo model_trans.sh"


if [ -z $2 ] || [ $2 != 'auto' ]; then
    # model_trans.sh  $1
    model_trans.sh YoloV5_s6_pytorch.json 
else  # only $2 == auto
    # get out path auto
    out_path=`cat $1 | grep out_path | cut -f4 -d '"'`
    # 1. build custom so
    if [ -d $out_path/opc/compile ]; then
        bash $out_path/opc/compile/build_so.sh
        if [ $? -ne 0 ]; then
            echo "[demo] ERROR: build custom layer to '.so' failed"
            exit 1
        fi
    fi

    # 2. generate inference images
    scale=1  # 1 letterbox, 0 resize
    hikflow_pre_imgs.sh $1 $scale
    if [ $? -ne 0 ]; then
        echo "[demo] ERROR: running preprocess img failed"
        exit 1
    fi

    cp -rf $out_path/../data $pwd_path/../inference && rm -rf $out_path/../data
    mv $out_path/../imagelist.txt $pwd_path/../inference/

    # 3. model transform
    model_trans.sh $1

    # 4. copy custom layer
    if [ "`ls -A $out_path/tctool`" != "" ] && [ -d $pwd_path/../inference/custom_layer ]; then
        cp $out_path/tctool/*.c $pwd_path/../inference/custom_layer
        cp $out_path/tctool/*.h $pwd_path/../inference/custom_layer
        if [ "`ls $out_path/tctool | grep -F .o`" != "" ]; then 
            cp $out_path/tctool/*.o $pwd_path/../inference/custom_layer
        fi
    fi 

    # 5. cp model files
    if [ -d $pwd_path/../inference ]; then
        cp -rf $out_path/bin $pwd_path/../inference
    fi
    
    # 6. delete temp files
    chmod -R 757 ../
fi

find . -name "__pycache__" | xargs rm -rf
echo "[demo] Done..."

exit 0
