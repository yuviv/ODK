#ifndef _NUMBER_CLASSIFIER_H_
#define _NUMBER_CLASSIFIER_H_

#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

#include "NumberClassification.h"
#include "SegmentMask.h"

typedef int (*filter_func)(const struct dirent *ent);
typedef int (*thresh_func)(int mean);

class NumberClassifier {
  protected:
    struct dirent **num_list;
    int n_numbers;
    int img_w;
    int img_h;
    cv::Mat h_mask;
    cv::Mat v_mask;
    const thresh_func t_func;
    std::vector<cv::Rect> rois;
    std::vector<int> guesses;
    std::vector<int> correct;
    const int total_pixels;
    NumberClassifier (const std::string classify_dir,
                      filter_func ff, mask_func mf, thresh_func tf, int iw, int ih, int mw, int mh) : 
                                                      img_w(iw), img_h(ih),
                                                      t_func(tf),
                                                      guesses(10, 0),
                                                      correct(10, 0),
                                                      h_mask(mw, mh, CV_8U, cv::Scalar::all(0)),
                                                      v_mask(mh, mw, CV_8U, cv::Scalar::all(0)),
                                                      rois(NUM_SEGMENTS), total_pixels(mw * mh) {
      n_numbers = scandir(classify_dir.c_str(), &num_list, ff, alphasort);
      for (int i = 0; i < NUM_SEGMENTS; i++) {
        find_roi(i, iw, ih, mw, mh);
      }
      for (int i = 0; i < mw; i++) {
        for (int j = 0; j < mh; j++) {
          h_mask.at<uchar>(i, j, 0) = (*mf)(i, j, mw, mh, HORIZONTAL_MASK);
          v_mask.at<uchar>(j, i, 0) = (*mf)(j, i, mw, mh, VERTICAL_MASK);
        }
      }
    }
    void find_roi(int segment, int iw, int ih, int mw, int mh);
    int predict_number(const char guess);
    void preproc_img(cv::Mat& img);
  public:
    virtual void classify_data(void) {};
    virtual void print_results(void) {};
    void print_rois(void);
};

#endif //_NUMBER_CLASSIFIER_H_
