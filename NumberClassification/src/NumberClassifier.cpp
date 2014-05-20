#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>

#include "NumberClassifier.h"

void NumberClassifier::find_roi(int segment, int iw, int ih, int mw, int mh) {
    int x, y,w,h;
    w = mh;
    h = mw;
    switch(segment) {
    case TOP:
        x = (iw / 2) - (mw / 2);
        y = (ih / 6) - (mh / 2);
        w = mw;
        h = mh;
        break;
    case MIDDLE:
        x = (iw / 2) - (mw / 2);
        y = (ih / 2) - (mh / 2);
        w = mw;
        h = mh;
        break;
    case BOTTOM:
        x = (iw / 2) - (mw / 2);
        y = 5 * (ih / 6) - (mh / 2);
        w = mw;
        h = mh;
        break;
    case TOP_LEFT:
        x = (iw / 4) - (mh / 2);
        y = (ih / 3) - (mw / 2);
        break;
    case TOP_RIGHT:
        x = 3 * (iw / 4) - (mh / 2);
        y = (ih / 3) - (mw / 2);
        break;
    case BOTTOM_LEFT:
        x = (iw / 4) - (mh / 2);
        y = 2 * (ih / 3) - (mw / 2);
        break;
    case BOTTOM_RIGHT:
        x = 3 * (iw / 4) - (mh / 2);
        y = 2 * (ih / 3) - (mw / 2);
        break;
    }
    rois.at(segment) = cv::Rect(x, y, w, h);
}

int NumberClassifier::predict_number(const char guess) {
    switch (guess) {
    case ZERO_SEGMENTS:
        return 0;
    case ONE_SEGMENTS_LEFT:
        return 1;
    case ONE_SEGMENTS_RIGHT:
        return 1;
    case TWO_SEGMENTS:
        return 2;
    case THREE_SEGMENTS:
        return 3;
    case FOUR_SEGMENTS:
        return 4;
    case FIVE_SEGMENTS:
        return 5;
    case SIX_SEGMENTS:
        return 6;
    case SIX_SEGMENTS_PARTIAL:
        return 6;
    case SEVEN_SEGMENTS:
        return 7;
    case EIGHT_SEGMENTS:
        return 8;
    case NINE_SEGMENTS:
        return 9;
    case NINE_SEGMENTS_PARTIAL:
        return 9;
    default:
        return -1;
    }
}

void NumberClassifier::print_rois(void) {
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        std::cout << "ROI #" << i << std::endl;
        std::cout << "Top left: (" << rois.at(i).tl().x << "," << rois.at(i).tl().y << ")" << std::endl;
        std::cout << "Bottom right: (" << rois.at(i).br().x << "," << rois.at(i).br().y << ")" << std::endl;
    }
}

void NumberClassifier::pre_process(cv::Mat& img) {
    cv::Mat img_gray = img;
    if (img.channels() > 1)
        cv::cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::resize(img_gray, img_gray, cv::Size(img_w,img_h), 0, 0, cv::INTER_AREA);
    std::vector<cv::Mat> channels;
    split(img, channels);
    int mean = (int)cv::mean(channels[0])[0];
    int thresh = (*t_func)(mean);
    cv::Mat img_bin;
    cv::threshold(img_gray, img_bin, thresh, 255, cv::THRESH_BINARY);
    img = img_bin;
}

void NumberClassifier::print_results(void) {
    int total_correct = 0;
    for (int i = 0; i < 10; i++) {
        std::cout << "#" << i << ": " << correct.at(i) << "/" << guesses.at(i) << std::endl;
        total_correct += correct.at(i);
    }
    std::cout << "Total: " << total_correct << "/" << c_numbers << std::endl;
}

void NumberClassifier::classify(void) {
    int n = c_numbers;
    if (n < 0) {
        std::cerr << "Error opening directory" << std::endl;
    }
    while (n--) {
        std::string img_name = std::string(c_list[n]->d_name);
        int img_num = img_name.at(0) - 0x30;
        cv::Mat img = cv::imread(c_dir + img_name, CV_LOAD_IMAGE_COLOR);
        if (img.empty())
            std::cerr << "Image not loaded" << std::endl;
        else {
            pre_process(img);
            char seg_bits = c_process(img);
            int guess = predict_number(seg_bits);
            if (img_num == guess)
                correct.at(img_num)++;
            guesses.at(img_num)++;
        }
        delete c_list[n];
    }
    delete c_list;
}
