#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>

#include "NumberClassifier.h"

void NumberClassifier::find_roi(int segment, int iw, int ih, int mw, int mh) {
  int x, y;
  switch(segment) {
    case TOP:
      x = (iw / 2) - (mw / 2);
      y = (ih / 4) - (mh / 2);
      break;
    case MIDDLE:
      x = (iw / 2) - (mw / 2);
      y = (ih / 6) - (mh / 2);
      break;
    case BOTTOM:
      x = (iw / 2) - (mw / 2);
      y = 5 * (ih / 6) - (mh / 2);
      break;
    case TOP_LEFT:
      x = (iw / 4) - (mw / 2);
      y = (ih / 3) - (mh / 2);
      break;
    case TOP_RIGHT:
      x = 3 * (iw / 4) - (mw / 2);
      y = (ih / 3) - (mh / 2);
      break;
    case BOTTOM_LEFT:
      x = (iw / 4) - (mw / 2);
      y = 2 * (ih / 3) - (mh / 2);
      break;
    case BOTTOM_RIGHT:
      x = 3 * (iw / 4) - (mw / 2);
      y = 2 * (ih / 3) - (mh / 2);
      break;
  }
  rois.at(segment) = cv::Rect(x, y, mw, mh);
}

int NumberClassifier::predict_number(const char guess) {
  switch (guess) {
    case ZERO_SEGMENTS:
      return 0;
    case ONE_SEGMENTS_LEFT:
      return 1;
    case ONE_SEGMENTS_RIGHT:
      return 1;
    case TWO_SEGMENTS:
      return 2;
    case THREE_SEGMENTS:
      return 3;
    case FOUR_SEGMENTS:
      return 4;
    case FIVE_SEGMENTS:
      return 5;
    case SIX_SEGMENTS:
      return 6;
    case SIX_SEGMENTS_PARTIAL:
      return 6;
    case SEVEN_SEGMENTS:
      return 7;
    case EIGHT_SEGMENTS:
      return 8;
    case NINE_SEGMENTS:
      return 9;
    case NINE_SEGMENTS_PARTIAL:
      return 9;
    default:
      return -1;
  } 
}

void NumberClassifier::print_rois(void) {
  cv::Point2i tl;
  cv::Point2i br;
  for (int i = 0; i < NUM_SEGMENTS; i++) {
    std::cout << "ROI #" << i << std::endl;
    std::cout << "Top left: (" << rois.at(i).tl().x << "," << rois.at(i).tl().y << ")" << std::endl;
    std::cout << "Bottom right: (" << rois.at(i).br().x << "," << rois.at(i).br().y << ")" << std::endl;
  }
}

void NumberClassifier::preproc_img(cv::Mat& img) {
  cv::cvtColor(img, img, CV_BGR2GRAY);
  cv::resize(img, img, cv::Size(img_w,img_h), 0, 0, cv::INTER_AREA);
}
