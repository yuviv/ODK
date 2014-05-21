#include "NonMLClassifier.h"

int NonMLClassifier::c_process(const cv::Mat& img) {
    char seg_bits = 0;
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        int black_pixels = get_black_pixels(img, i);
        if (black_pixels > pixel_thresh)
            seg_bits |= (1 << i);
    }
    return predict_number(seg_bits);
}
