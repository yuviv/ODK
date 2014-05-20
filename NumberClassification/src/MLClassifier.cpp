#include "MLClassifier.h"

void MLClassifier::train(void) {
    int n = t_numbers;
    if (n < 0) {
        std::cerr << "Error opening directory" << std::endl;
    }
    while (n--) {
        std::string img_name = std::string(t_list[n]->d_name);
        int img_num = img_name.at(0) - 0x30;
        cv::Mat img = cv::imread(c_dir + img_name, CV_LOAD_IMAGE_COLOR);
        if (img.empty())
            std::cerr << "Image not loaded" << std::endl;
        else {
            pre_process(img);
            t_process(img, img_num);
        }
        delete c_list[n];
    }
    delete c_list;
}
