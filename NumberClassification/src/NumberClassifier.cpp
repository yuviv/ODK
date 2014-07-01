#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>

#include "NumberClassifier.h"

/* Calculates the location and dimensions of each segment's rectangular
 * boundary based on the size of the image and the size of the segments.
 * Assumes that the image is aligned, and centers each segment on the midpoint
 * of the imaginary line between dots */
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

/* Used to map the output of c_process() to a number */
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

/* Prints out the segment locations and boundaries */
void NumberClassifier::print_rois(void) {
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        std::cout << "ROI #" << i << std::endl;
        std::cout << "Top left: (" << rois.at(i).tl().x << "," << rois.at(i).tl().y << ")" << std::endl;
        std::cout << "Bottom right: (" << rois.at(i).br().x << "," << rois.at(i).br().y << ")" << std::endl;
    }
}

/* Searches for the six dots and uses their location to crop and align the image */
void NumberClassifier::crop_img(cv::Mat& img, int target_w, int target_h) {
  int max_x_off = 0;
  int max_y_off = 0;
  int dot_w = target_w / 2;
  int dot_h = 2 * target_h / 3;
  int min_px = 25;
  int img_w = img.cols;
  int img_h = img.rows;
  int total_px;
  cv::Rect r;
  for (int i = 0; i < img_w - dot_w - 2; i++) {
    for (int j = 0; j < img_h - dot_h - 2; j++) {
      total_px = 0;
      r = cv::Rect(i, j, 2, 2);
      total_px += cv::countNonZero(img(r));
      r = cv::Rect(i + dot_w, j, 2, 2);
      total_px += cv::countNonZero(img(r));
      r = cv::Rect(i, j + dot_h / 2, 2, 2);
      total_px += cv::countNonZero(img(r));
      r = cv::Rect(i + dot_w, j + dot_h / 2, 2, 2);
      total_px += cv::countNonZero(img(r));
      r = cv::Rect(i, j + dot_h, 2, 2);
      total_px += cv::countNonZero(img(r));
      r = cv::Rect(i + dot_w, j + dot_h, 2, 2);
      total_px += cv::countNonZero(img(r));
      if (total_px < min_px) {
        min_px = total_px;
        max_x_off = i - target_w / 4 + 1;
        max_y_off = j - target_h / 6 + 1;
      }
    }
  }
  if (!(max_x_off + target_w < img_w)) {
    target_w -= ((max_x_off + target_w) - img_w + 1);
  } else if (max_x_off < 0) {
    target_w += max_x_off;
    max_x_off = 0;
  }
  if (!(max_y_off + target_h < img_h)) {
    target_h -= ((max_y_off + target_h) - img_h + 1);
  } else if (max_y_off < 0) {
    target_h += max_y_off;
    max_y_off = 0;
  }
  img = img(cv::Rect(max_x_off, max_y_off, target_w, target_h));
}

/* Converts the image to black/white, crops it, resizes it */
void NumberClassifier::pre_process(cv::Mat& img) {
    cv::Mat img_gray = img;
    if (img.channels() > 1)
        cv::cvtColor(img, img_gray, CV_BGR2GRAY);
    std::vector<cv::Mat> channels;
    split(img, channels);
    int mean = (int)cv::mean(channels[0])[0];
    int thresh = (*t_func)(mean);
    cv::Mat img_bin;
    cv::threshold(img_gray, img_bin, thresh, 255, cv::THRESH_BINARY);
    img = img_bin;
    crop_img(img, img_w, img_h);
    cv::resize(img, img, cv::Size(img_w,img_h), 0, 0, cv::INTER_AREA);
}

/* Prints the results of the classification */
void NumberClassifier::print_results(void) {
    int total_correct = 0;
    for (int i = 0; i < 10; i++) {
        std::cout << "#" << i << ": " << correct.at(i) << "/" << guesses.at(i) << std::endl;
        total_correct += correct.at(i);
    }
    std::cout << "Total: " << total_correct << "/" << c_numbers << std::endl;
}

/* Iterates through the classification directory, passing each image to the
 * c_process() function which performs the actual classification */
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
            int guess = c_process(img);
            if (img_num == guess)
                correct.at(img_num)++;
            guesses.at(img_num)++;
        }
        delete c_list[n];
    }
    delete c_list;
}

/* Counts the number of black pixels in the given segment of the image */
int NumberClassifier::get_black_pixels(const cv::Mat& img, int segment) {
    cv::Mat img_segment = img(rois.at(segment));
    if ((1 << segment) & (TOP_BIT | MIDDLE_BIT | BOTTOM_BIT)) {
        bitwise_and(img_segment, h_mask.get_mask(segment), img_segment);
    } else if ((1 << segment) & (TOP_LEFT_BIT | TOP_RIGHT_BIT | BOTTOM_LEFT_BIT | BOTTOM_RIGHT_BIT)) {
        bitwise_and(img_segment, v_mask.get_mask(segment), img_segment);
    }
    return total_pixels - cv::countNonZero(img_segment);
}
