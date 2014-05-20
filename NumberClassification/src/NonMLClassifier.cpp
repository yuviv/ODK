#include "NonMLClassifier.h"

char NonMLClassifier::c_process(const cv::Mat& img) {
    char seg_bits = 0;
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        check_segment(img, i, &seg_bits);
    }
    return seg_bits;
}

void NonMLClassifier::check_segment(const cv::Mat& img, int segment, char *seg_bits) {
    cv::Mat img_segment = img(rois.at(segment));
    if ((1 << segment) & (TOP_BIT | MIDDLE_BIT | BOTTOM_BIT)) {
        bitwise_and(img_segment, h_mask.get_mask(segment), img_segment);
    } else if ((1 << segment) & (TOP_LEFT_BIT | TOP_RIGHT_BIT | BOTTOM_LEFT_BIT | BOTTOM_RIGHT_BIT)) {
        bitwise_and(img_segment, v_mask.get_mask(segment), img_segment);
    }
    int black_pixels = total_pixels - countNonZero(img_segment);
    if (black_pixels > pixel_thresh) {
        *seg_bits |= (1 << segment);
    }
}
