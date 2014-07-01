#ifndef _NAIVE_BAYES_H_
#define _NAIVE_BAYES_H_

#include "MLClassifier.h"
#include "PixelStats.h"

/* This class implements a Naive Bayes classifier. It is an extension
 * of the abstract MLClassifier class, and it adds one additional state
 * component (stats). The stats object retains the mean and variance of
 * the number of pixels in each segment for each number when training,
 * and those values are used after training to classify images. */
class NaiveBayes : public MLClassifier
{
protected:
    PixelStats stats;
    void t_process(const cv::Mat& img, int img_num);
    int c_process(const cv::Mat &img);
public:
    NaiveBayes(const std::string classify_dir, std::string train_dir,
               filter_func ff, mask_func mf, thresh_func tf,
               int iw, int ih, int mw, int mh):
        MLClassifier(classify_dir, train_dir, ff, mf, tf, iw, ih, mw, mh) {}
};

#endif // _NAIVE_BAYES_H_
