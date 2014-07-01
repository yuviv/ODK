#include <opencv/highgui.h>

#include "MLClassifier.h"

/* This function iterates through a training directory and calls
 * t_process() on each image in the directory. Can be used by any
 * ML classifier as a generic train function */
void MLClassifier::train(void) {
    int n = t_numbers;
    if (n < 0) {
        std::cerr << "Error opening directory" << std::endl;
    }
    while (n--) {
        std::string img_name = std::string(t_list[n]->d_name);
        int img_num = img_name.at(0) - 0x30;
        cv::Mat img = cv::imread(t_dir + img_name, CV_LOAD_IMAGE_COLOR);
        if (img.empty())
            std::cerr << "Image not loaded" << std::endl;
        else {
            pre_process(img);
            t_process(img, img_num);
        }
        delete t_list[n];
    }
    delete t_list;
}
