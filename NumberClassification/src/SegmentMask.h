#ifndef _SEGMENT_MASK_H_
#define _SEGMENT_MASK_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "NumberClassification.h"

#define HORIZONTAL_MASK 0
#define VERTICAL_MASK 1

typedef int (*mask_func)(int x, int y, int w, int h, int dir);

class SegmentMask {
  protected:
    mask_func func;
    cv::Mat mask;
  public:
    SegmentMask(const mask_func mf, int w, int h, int dir) : func(mf), mask(h, w, CV_8U, cv::Scalar::all(0)) {
      for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
          mask.at<uchar>(i, j, 0) = (*func)(i, j, w, h, dir);
        }
      }
    }
    const cv::Mat& get_mask(const char segment) {
      return mask;
    }
};

#endif //_SEGMENT_MASK_H_
