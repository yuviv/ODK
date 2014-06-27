#include <iostream>
#include <cstring>
#include <sstream>
#include <cmath>

#include "driver.h"
#include "NonMLClassifier.h"
#include "NaiveBayes.h"

int main(int argc, char *argv[]) {
    int mw, mh, pt;

    if (argc != 6) {
        std::cerr << "Use: ./driver [train_dir] [classify_dir] [mask_width] [mask_height] [pixel_thresh]" << std::endl;
        return -1;
    } else {
        int val;
        for (int i = 3; i <= 5; i++) {
            std::istringstream iss(argv[i]);
            if (iss >> val) {
                switch(i) {
                case 3:
                    mw = val;
                    break;
                case 4:
                    mh = val;
                    break;
                case 5:
                    pt = val;
                    break;
                default:
                    return -1;
                }
            }
        }
    }

    std::string c_dir(argv[1]);
    std::string t_dir(argv[2]);

    //NonMLClassifier classifier(c_dir, &filter, &rect_mask, &thresh, 40, 60, mw, mh, pt);
    //classifier.classify();
    //classifier.print_results();

    NaiveBayes naive_bayes(c_dir, t_dir, &filter, &rect_mask, &thresh, 40, 60, mw, mh);
    naive_bayes.train();
    naive_bayes.classify();
    naive_bayes.print_results();
}

int filter(const struct dirent *ent) {
    const char *file_name = ent->d_name;
    const char *jpeg = ".jpg";
    return !!strstr(file_name, jpeg);
}

int rect_mask(int x, int y, int w, int h, int dir) {
    return 1;
}

int ellipse_mask(int x, int y, int w, int h, int dir) {
    int width = dir ? h : w;
    int height = dir ? w : h;
    int rx = width/2;
    int ry = height/2;
    int px = dir ? y-rx : x-rx;
    int py = dir ? x-ry : y-ry;
    px += (px < 0) ? (1 - width % 2) : 0;
    py += (py < 0) ? (1 - height % 2) : 0;
    rx -= (!(width % 2)) ? 1 : 0;
    ry -= (!(width % 2)) ? 1 : 0;
    double sqx = pow(1.0 * (px) / rx, 2);
    double sqy = pow(1.0 * (py) / ry, 2);
    return (sqx + sqy <= 1) ? 1 : 0;
}

int diamond_mask(int x, int y, int w, int h, int dir) {
    int width = dir ? h : w;
    int height = dir ? w : h;
    int rx = width / 2;
    int ry = height / 2;
    int px = dir ? y-rx : x-rx;
    int py = dir ? x-ry : y-ry;
    px += (px < 0) ? (1 - width % 2) : 0;
    py += (py < 0) ? (1 - height % 2) : 0;
    rx -= (!(width % 2)) ? 1 : 0;
    ry -= (!(height % 2)) ? 1 : 0;
    if (!(py && px))
        return 1;
    else if (py > 0 && px > 0)
        return (py <= ry - ry * px / rx) ? 1 : 0;
    else if (py > 0 && px < 0)
        return (py <= ry + ry * px / rx) ? 1 : 0;
    else if (py < 0 && px > 0)
        return (py >= -ry + ry * px / rx) ? 1 : 0;
    else
        return (py >= -ry - ry * px / rx) ? 1 : 0;
}

int thresh(int mean) {
    return mean;
}
