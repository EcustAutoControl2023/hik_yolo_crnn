#include "plate_trans.h"

void debugShow(cv::Mat &img, std::string name)
{
  cv::imshow(name, img);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

void getLicensePlateCandidates(cv::Mat &gray, std::vector<std::vector<cv::Point>> &out_contours, int keep=5, bool debug=false)
{
  cv::Mat rectKern = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(13, 5));
  cv::Mat blackhat;
  cv::morphologyEx(gray, blackhat, cv::MORPH_BLACKHAT, rectKern);
  if (debug) debugShow(blackhat, "blackhat");

  cv::Mat squareKern = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(12, 12));
  cv::Mat light;
  cv::morphologyEx(gray, light, cv::MORPH_CLOSE, squareKern);
  double ret = cv::threshold(light, light, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  if (debug) debugShow(light, "light");

  cv::Mat gradX;
  cv::Sobel(blackhat, gradX, CV_32F, 1, 0, -1);
  cv::convertScaleAbs(gradX, gradX);

  double minVal, maxVal;
  cv::minMaxLoc(gradX, &minVal, &maxVal);

  gradX = 255 * (gradX - minVal) / (maxVal - minVal);
  gradX.convertTo(gradX, CV_8U);
  if (debug) debugShow(gradX, "Scharr");

  cv::GaussianBlur(gradX, gradX, cv::Size(5, 5), 0);
  cv::morphologyEx(gradX, gradX, cv::MORPH_CLOSE, rectKern);

  cv::Mat thresh;
  cv::threshold(gradX, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  if (debug) debugShow(thresh, "Grad Thresh");

  cv::erode(thresh, thresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
  cv::dilate(thresh, thresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
  if (debug) debugShow(thresh, "Grad Erode/Dilate");

  cv::bitwise_and(thresh, light, thresh);
  cv::dilate(thresh, thresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
  cv::erode(thresh, thresh, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
  if (debug) debugShow(thresh, "Final");

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // sort contours by area, reversed=true
  std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
    return cv::contourArea(a) > cv::contourArea(b);
  });

  // keep top 5 contours
  contours.resize(std::min(keep, (int)contours.size()));

  // debug printf("\n");
  if (debug)
  {
    for (int i = 0; i < contours.size(); i++)
    {
      cv::RotatedRect rect = cv::minAreaRect(contours[i]);
      cv::Point2f vertices[4];
      rect.points(vertices);

      // draw points
      cv::drawContours(gray, contours, i, cv::Scalar(0, 255, 0), 2);
      printf("Contour %d: %.2f\n", i, cv::contourArea(contours[i])); 
      for (int j = 0; j < 4; j++)
      {
        printf("  %.2f, %.2f\n", vertices[j].x, vertices[j].y);
        // cv::drawMarker(gray, vertices[j], cv::Scalar(0, 255, 0), cv::MARKER_CROSS, 10, 2);

        cv::line(gray, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
      }
      debugShow(gray, "Contour");
    }
  }

  out_contours = contours;
}

void getFeaturePoint(cv::Mat gray, std::vector<cv::Point> contour, std::vector<cv::Point> &feature_point, int delta=7, bool debug=false)
{
  cv::Point p1(-100, -100), 
            p2(-100, -100), 
            p3(-100, -100), 
            p4(-100, -100);
  cv::Rect rect = cv::boundingRect(contour);
  cv::Point bp1(rect.x, rect.y), 
            bp2(rect.x + rect.width, rect.y), 
            bp3(rect.x, rect.y + rect.height), 
            bp4(rect.x + rect.width, rect.y + rect.height);

  if (debug) {
    printf("bp1: (%d, %d)\n", bp1.x, bp1.y);
    printf("bp2: (%d, %d)\n", bp2.x, bp2.y);
    printf("bp3: (%d, %d)\n", bp3.x, bp3.y);
    printf("bp4: (%d, %d)\n", bp4.x, bp4.y);
    cv::rectangle(gray, rect, cv::Scalar(0, 255, 0), 2);
  }

  for (auto point : contour) {
    if (cv::norm(point - bp1) < cv::norm(p1 - bp1)) {
      p1 = point;
    }
    if (cv::norm(point - bp2) < cv::norm(p2 - bp2)) {
      p2 = point;
    }
    if (cv::norm(point - bp3) < cv::norm(p3 - bp3)) {
      p3 = point;
    }
    if (cv::norm(point - bp4) < cv::norm(p4 - bp4)) {
      p4 = point;
    }
  }

  p1.x = p1.x - delta < 0 ? 0 : p1.x - delta;
  p1.y = p1.y - delta < 0 ? 0 : p1.y - delta;
  p2.x = p2.x + delta > gray.cols ? gray.cols : p2.x + delta;
  p2.y = p2.y - delta < 0 ? 0 : p2.y - delta;
  p3.x = p3.x - delta < 0 ? 0 : p3.x - delta;
  p3.y = p3.y + delta > gray.rows ? gray.rows : p3.y + delta;
  p4.x = p4.x + delta > gray.cols ? gray.cols : p4.x + delta;
  p4.y = p4.y + delta > gray.rows ? gray.rows : p4.y + delta;

  if (debug) {
    printf("p1: (%d, %d)\n", p1.x, p1.y);
    printf("p2: (%d, %d)\n", p2.x, p2.y);
    printf("p3: (%d, %d)\n", p3.x, p3.y);
    printf("p4: (%d, %d)\n", p4.x, p4.y);
    cv::circle(gray, p1, 5, cv::Scalar(0, 0, 255), -1);
    cv::circle(gray, p2, 5, cv::Scalar(0, 0, 255), -1);
    cv::circle(gray, p3, 5, cv::Scalar(0, 0, 255), -1);
    cv::circle(gray, p4, 5, cv::Scalar(0, 0, 255), -1);
  }

  feature_point[0] = p1;
  feature_point[1] = p2;
  feature_point[2] = p4;
  feature_point[3] = p3;
}

void getTransForm(cv::Mat gray, cv::Mat &dst_img, std::vector<cv::Point> feature_point, bool debug=false)
{
  cv::Point2f src[4], dst[4];
  src[0] = feature_point[0];
  src[1] = feature_point[1];
  src[2] = feature_point[2];
  src[3] = feature_point[3];
  dst[0] = cv::Point2f(0, 0);
  dst[1] = cv::Point2f(gray.cols, 0);
  dst[2] = cv::Point2f(gray.cols, gray.rows);
  dst[3] = cv::Point2f(0, gray.rows);
  cv::Mat M = cv::getPerspectiveTransform(src, dst);
  cv::warpPerspective(gray, dst_img, M, cv::Size(gray.cols, gray.rows));
  if (debug) debugShow(dst_img, "Transform");
}
