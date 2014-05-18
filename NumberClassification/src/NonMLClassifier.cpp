#include <opencv/highgui.h>

#include "NonMLClassifier.h"

void NonMLClassifier::classify_data() {
  int n = c_numbers;
  if (n < 0) {
    std::cerr << "Error opening directory" << std::endl;
  }
  while (n--) {
    std::string img_name = std::string(c_list[n]->d_name);
    std::cout << img_name << std::endl;
    int img_num = img_name.at(0) - 0x30;
    char seg_bits = 0;
    cv::Mat img = cv::imread(c_dir + img_name, CV_LOAD_IMAGE_COLOR);
    if (img.empty())
      std::cerr << "Image not loaded" << std::endl;
    for (int i = 0; i < NUM_SEGMENTS; i++) {
      preproc_img(img);
      proc_seg(img, i, &seg_bits);
    }
    int guess = predict_number(seg_bits); 
    if (img_num == guess)
      correct.at(img_num)++;
    guesses.at(img_num)++;
  }
}

void NonMLClassifier::print_results() {
  for (int i = 0; i < 10; i++) {
    std::cout << "#" << i << ": " << correct.at(i) << "/" << guesses.at(i) << std::endl;
  }
}

void NonMLClassifier::proc_seg(const cv::Mat& img, int segment, char *seg_bits) {
  cv::Mat img_segment = img(rois.at(segment));
  if ((1 << segment) & (TOP_BIT | MIDDLE_BIT | BOTTOM_BIT)) {
    bitwise_and(img_segment, h_mask.get_mask(segment), img_segment);
  } else if ((1 << segment) & (TOP_LEFT_BIT | TOP_RIGHT_BIT | BOTTOM_LEFT_BIT | BOTTOM_RIGHT_BIT)) {
    bitwise_and(img_segment, v_mask.get_mask(segment), img_segment);
  }
  if ((total_pixels - countNonZero(img_segment)) > pixel_thresh) {
    *seg_bits |= (1 << segment);
  }  
}
