#include "NaiveBayes.h"

void NaiveBayes::t_process(const cv::Mat& img, int img_num) {
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        int black_pixels = get_black_pixels(img, i);
        stats.add_seg(img_num, i, black_pixels);
    }
}

int NaiveBayes::c_process(const cv::Mat& img) {
    float max_prod = -1.0;
    int max_num;
    float prod;
    for (int num = 0; num < 10; num++) {
        prod = 1.0;
        for (int seg = 0; seg < NUM_SEGMENTS; seg++) {
            int black_pixels = get_black_pixels(img, seg);
            prod *= stats.get_prob(num, seg, black_pixels);
        }
        if (prod > max_prod) {
            max_prod = prod;
            max_num = num;
        }
    }
    return max_num;
}
