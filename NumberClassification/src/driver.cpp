#include <iostream>
#include "driver.h"
#include "NonMLClassifier.h"
#include <cstring>
#include <sstream>
#include <cmath>

int main(int argc, char *argv[]) {
    int mw, mh, pt;

    if (argc != 4) {
        std::cerr << "Use: ./driver [mask_width] [mask_height] [pixel_thresh]" << std::endl;
        return -1;
    } else {
        int val;
        for (int i = 1; i <= 3; i++) {
            std::istringstream iss(argv[i]);
            if (iss >> val) {
                switch(i) {
                case 1:
                    mw = val;
                    break;
                case 2:
                    mh = val;
                    break;
                case 3:
                    pt = val;
                    break;
                default:
                    return -1;
                }
            }
        }
    }

    NonMLClassifier classifier("./natural_numbers/", &filter, &ellipse_mask, &thresh, 20, 30, mw, mh, pt);
    classifier.classify();
    classifier.print_results();
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
