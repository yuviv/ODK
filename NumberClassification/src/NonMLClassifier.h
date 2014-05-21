#ifndef _NON_ML_CLASSIFIER_H_
#define _NON_ML_CLASSIFIER_H_

#include "NumberClassifier.h"

class NonMLClassifier: public NumberClassifier
{
protected:
    int pixel_thresh;
    void check_segment(const cv::Mat& img, int segment, char *seg_bits);
public:
    NonMLClassifier(const std::string classify_dir, filter_func ff, mask_func mf, thresh_func tf,
                    int iw, int ih, int mw, int mh, int thresh):
        NumberClassifier(classify_dir, ff, mf, tf, iw, ih, mw, mh), pixel_thresh(thresh) {}
    int c_process(const cv::Mat& img);
};

#endif //_NON_ML_CLASSIFIER_H_
