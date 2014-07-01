#ifndef _NUMBER_CLASSIFIER_H_
#define _NUMBER_CLASSIFIER_H_

#include <dirent.h>
#include <vector>
#include <string>

#include "NumberClassification.h"
#include "SegmentMask.h"

typedef int (*filter_func)(const struct dirent *ent);
typedef int (*thresh_func)(int mean);

/* This a virtual class that performs all the basic preprocessing
 * that the other derived classes use. It also does not provide
 * any form of training; this was done to support classifiers that
 * do not learn (such as the threshold example). */
class NumberClassifier
{
protected:
    struct dirent **c_list;     // list of image names
    const std::string c_dir;    // name of classify directory
    int c_numbers;              // number of images to classify
    int img_w;                  // images resized to this width
    int img_h;                  // images resized to this height
    SegmentMask h_mask;         // defines a mask shape for the vertical segments
    SegmentMask v_mask;         // defines a mask shape for the horiz. segments
    const thresh_func t_func;   // calculates the pixel intensity threshold
    std::vector<cv::Rect> rois; // rectangles for the segment locations
    std::vector<int> guesses;   // keeps track of guesses
    std::vector<int> correct;   // keeps track of correct guesses
    const int total_pixels;     // total pixels in each segment
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
