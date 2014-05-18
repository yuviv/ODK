#include "NonMLClassifier.h"

void NonMLClassifier::classify_data() {
  
}

void NonMLClassifier::print_results() {

}

void NonMLClassifier::process_segment(const cv::Mat& img, int segment, char *guess) {
  cv::Mat img_segment = img(rois.at(segment));
  if (segment & (TOP_BIT | MIDDLE_BIT | BOTTOM_BIT)) {
    bitwise_and(img_segment, h_mask, img_segment);
  } else if (segment & (TOP_LEFT_BIT | TOP_RIGHT_BIT | BOTTOM_LEFT_BIT | BOTTOM_RIGHT_BIT)) {
    bitwise_and(img_segment, v_mask, img_segment);
  }
  if ((total_pixels - countNonZero(img_segment)) > pixel_thresh) {
    *guess |= (1 << segment);
  }  
}
