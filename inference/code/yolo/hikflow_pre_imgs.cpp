#include <math.h>
#include "hikflow_pre_imgs.h"


void letterbox(
    cv::Mat* img, 
    unsigned int width,
    unsigned int height,
    unsigned int bgColor,
    bool _auto,
    bool scaleFill,
    bool scaleup,
    unsigned int stride
) {
  int shape[2] = {img->cols, img->rows};
  int dw = 0;
  int dh = 0;
  int new_unpad[2] = {0, 0};
  float r = 0.0;
  cv::Mat temp;
  int top, bottom, left, right;

  r = MIN((float)width / (float)shape[0], (float)height / (float)shape[1]);
  if (!scaleup)
  {
    r = MIN(r, 1.0);
  }

  new_unpad[0] = (int)(round(shape[0] * r));
  new_unpad[1] = (int)(round(shape[1] * r));
  dw = width - new_unpad[0];
  dh = height - new_unpad[1];

  if (_auto)
  {
    dw = dw % stride;
    dh = dh % stride;
  }
  else if (scaleFill)
  {
    dw = 0.0;
    dh = 0.0;
    new_unpad[0] = width;
    new_unpad[1] = height;
  }

  dw = (float)dw / 2;
  dh = (float)dh / 2;

  // DEMOPRT((char *)"dw: %d, dh: %d\n", dw, dh);

  if (shape[0] != new_unpad[1] || shape[1] != new_unpad[0])
  {
    cv::resize(*img, temp, cv::Size(new_unpad[0], new_unpad[1]), 0, 0, cv::INTER_LINEAR);
  }

  top = (int)(round(dh - 0.1));
  bottom = (int)(round(dh + 0.1));
  left = (int)(round(dw - 0.1));
  right = (int)(round(dw + 0.1));

  // make sure new image size exact the given size
  if (top + bottom + new_unpad[1] != height)
  {
    if (top + bottom + new_unpad[1] < height)
    {
      top = top + 1;
    }
    else
    {
      top = top - 1;
    }
  }
  if (left + right + new_unpad[0] != width)
  {
    if (left + right + new_unpad[0] < width)
    {
      left = left + 1;
    }
    else
    {
      left = left - 1;
    }
  }

  cv::copyMakeBorder(temp, *img, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(bgColor, bgColor, bgColor));

}

void _resize_img(
  cv::Mat *img, 
  const char *img_path, 
  bool size_change, 
  CONFIG_DATA_T *cfg
)
{
  if (NULL == img_path)
  {
    DEMOPRT((char *)"img_path is NULL \n");
  }
  else 
  {
    *img = cv::imread(img_path);
    if (img->empty())
    {
      DEMOPRT((char *)"img is empty \n");
    }
    else
    {
      if (size_change)
      {
        // TODO: resize mode
        letterbox(img, cfg->w, cfg->h, 114, true, false , true, 1000);
      }
    }
  }

}

void mat_to_bgr_planner(
  cv::Mat *img, 
  const char *img_path, 
  const char *bgr_path, 
  bool size_change, 
  bool bgr_planner, 
  CONFIG_DATA_T *config_data
)
{
  _resize_img(img, img_path, size_change, config_data);
  int height = img->rows;
  int width = img->cols;
  int channel = img->channels();
  // DEMOPRT((char *)"height: %d, width: %d, channel: %d\n", height, width, channel);

  // 直接写入文件
  FILE *file = fopen(bgr_path, "w");
  // 格式按照channel写入bbbbb,ggggg,rrrrr, unsigned char
  cv::Mat channels_data[3];
  cv::split(*img, channels_data);

  for (int c=0; c < channel; c++)
    for (int i=0; i < width * height; i++)
      fprintf(file, "%c", (unsigned char)channels_data[c].data[i]);

  fclose(file);

}
