pwd_path=$(readlink -f "$(dirname "$0")")
status=$?
# third project environment configuration
export PYTHONPATH=$pwd_path/SourceProject/yolov5-6.0:$PYTHONPATH

echo "[demo] start running demo model_trans.sh"

echo $1
echo $2

# 如果$1中有Yolo则后缀为yolo，若有RCNN则后缀为rcnn
if [[ $1 == *"Yolo"* ]]; then
  suffix="yolo"
elif [[ $1 == *"RCNN"* ]]; then
  suffix="rcnn"
else
  echo "Invalid argument. Please provide either 'Yolo' or 'RCNN'."
  exit 1
fi

echo "suffix: $suffix"

if [ -z $2 ] || [ $2 != 'auto' ]; then
    # model_trans.sh  $1
    # model_trans.sh YoloV5_s6_pytorch.json 
    echo "[demo] ERROR: invalid argument, please provide 'auto'"
    exit 1
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

    if [ -d $pwd_path/../inference/data/$suffix ]; then
        echo "[demo] remove old data folder..."
        rm -rf $pwd_path/../inference/data/$suffix
    fi
    image_txt_name="imagelist_$suffix.txt"
    if [ -f $pwd_path/../inference/$image_txt_name ]; then
        echo "[demo] remove old image txt file..."
        rm -rf $pwd_path/../inference/$image_txt_name
    fi
    cp -rf $out_path/../data $pwd_path/../inference/data/$suffix && rm -rf $out_path/../data
    mv $out_path/../imagelist.txt $pwd_path/../inference/$image_txt_name

    # 3. model transform
    model_trans.sh $1

    # 4. copy custom layer
    if [ -d $pwd_path/../inference/custom_layer/$suffix ]; then
        echo "[demo] remove old custom layer files..."
        rm -rf $pwd_path/../inference/custom_layer/$suffix/*
    fi

    if [ "`ls -A $out_path/tctool`" != "" ] && [ -d $pwd_path/../inference/custom_layer/$suffix ]; then
        cp $out_path/tctool/*.c $pwd_path/../inference/custom_layer/$suffix
        cp $out_path/tctool/*.h $pwd_path/../inference/custom_layer/$suffix
        cp $out_path/opc/*.c $pwd_path/../inference/custom_layer/$suffix
        cp $out_path/opc/*.h $pwd_path/../inference/custom_layer/$suffix
        if [ "`ls $out_path/tctool | grep -F .o`" != "" ]; then 
            cp $out_path/tctool/*.o $pwd_path/../inference/custom_layer/$suffix
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
