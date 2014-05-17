#include "NonMLClassifier.h"

void NonMLClassifier::classify_data() {

}

void NonMLClassifier::print_results() {

}

void NonMLClassifier::process_segment(int x, int y, int w, int h) {
  cv::Rect r(x, y, w, h);
  cv::Mat segment = current_img(r);
}
