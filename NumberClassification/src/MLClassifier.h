#ifndef _ML_CLASSIFIER_H_
#define _ML_CLASSIFIER_H_

#include "NumberClassifier.h"

/* This abstract class extends the generic NumberClassifier by providing a
 * train() function and a virtual t_process() function. The train()
 * function simply iterates through a directory, passing each image
 * it finds to the t_process() function. The t_process() function is
 * implementation-specific. */
class MLClassifier : public NumberClassifier
{
protected:
    struct dirent **t_list;
    const std::string t_dir;
    int t_numbers;
    virtual void t_process(const cv::Mat& img, int img_num) {}
public:
    MLClassifier(const std::string classify_dir,
                 const std::string train_dir,
                 filter_func ff, mask_func mf,
                 thresh_func tf, int iw, int ih,
                 int mw, int mh):
        NumberClassifier(classify_dir, ff, mf, tf, iw, ih, mw, mh),
        t_dir(train_dir) {
        t_numbers = scandir(train_dir.c_str(), &t_list, ff, alphasort);
    }
    void train(void);
};

#endif // _ML_CLASSIFIER_H_
