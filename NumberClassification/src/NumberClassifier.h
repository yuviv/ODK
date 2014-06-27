#ifndef _NUMBER_CLASSIFIER_H_
#define _NUMBER_CLASSIFIER_H_

#include <dirent.h>
#include <vector>
#include <string>

#include "NumberClassification.h"
#include "SegmentMask.h"

typedef int (*filter_func)(const struct dirent *ent);
typedef int (*thresh_func)(int mean);

class NumberClassifier
{
protected:
    struct dirent **c_list;
    const std::string c_dir;
    int c_numbers;
    int img_w;
    int img_h;
    SegmentMask h_mask;
    SegmentMask v_mask;
    const thresh_func t_func;
    std::vector<cv::Rect> rois;
    std::vector<int> guesses;
    std::vector<int> correct;
    const int total_pixels;
    NumberClassifier (const std::string classify_dir, filter_func ff,
                      mask_func mf, thresh_func tf, int iw, int ih,
                      int mw, int mh) :
        c_dir(classify_dir), img_w(iw), img_h(ih),
        t_func(tf), guesses(10, 0), correct(10, 0),
        h_mask(mf, mw, mh, HORIZONTAL_MASK), v_mask(mf, mh, mw, VERTICAL_MASK),
        rois(NUM_SEGMENTS), total_pixels(h_mask.get_mask_area()) {
        c_numbers = scandir(classify_dir.c_str(), &c_list, ff, alphasort);
        for (int i = 0; i < NUM_SEGMENTS; i++) {
            find_roi(i, iw, ih, mw, mh);
        }
    }
    void find_roi(int segment, int iw, int ih, int mw, int mh);
    int predict_number(const char guess);
    void pre_process(cv::Mat& img);
    virtual int c_process(const cv::Mat& img) {}
    int get_black_pixels(const cv::Mat& img, int segment); 
    void crop_img(cv::Mat& img, int target_w, int target_h);
public:
    void classify(void);
    void print_results(void);
    void print_rois(void);
};

#endif //_NUMBER_CLASSIFIER_H_
