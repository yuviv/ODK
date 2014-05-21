#ifndef _NAIVE_BAYES_H_
#define _NAIVE_BAYES_H_

#include "MLClassifier.h"

class NaiveBayes : public MLClassifier
{
protected:
    void t_process(const cv::Mat& img, int img_num);
    int c_process(const cv::Mat &img);
public:
    NaiveBayes(const std::string classify_dir, std::string train_dir,
               filter_func ff, mask_func mf, thresh_func tf,
               int iw, int ih, int mw, int mh):
        MLClassifier(classify_dir, train_dir, ff, mf, tf, iw, ih, mw, mh) {}
};

#endif // _NAIVE_BAYES_H_
