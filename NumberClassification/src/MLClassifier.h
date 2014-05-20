#ifndef MLCLASSIFIER_H
#define MLCLASSIFIER_H

#include "NumberClassifier.h"

class MLClassifier : public NumberClassifier
{
protected:
    struct dirent **t_list;
    const std::string t_dir;
    int t_numbers;
public:
    MLClassifier(const std::string classify_dir,
                 const std::string train_dir,
                 filter_func ff, mask_func mf,
                 thresh_func tf, int iw, int ih,
                 int mw, int mh):
        NumberClassifier(classify_dir, ff, mf, tf, iw, ih, mw, mh),
        t_dir(train_dir) {
        c_numbers = scandir(train_dir.c_str(), &t_list, ff, alphasort);
    }
    void train(void);
    virtual void t_process(const cv::Mat& img, int img_num);
};

#endif // MLCLASSIFIER_H
