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
  classifier.classify_data();
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
  int rx = dir ? h>>1 : w>>1;
  int ry = dir ? w>>1 : h>>1;
  int px = dir ? (h-1)-y : x;
  int py = dir ? x : (h-1)-y;
  double sqx = pow(1.0 * (px-rx) / rx, 2.0);
  double sqy = pow(1.0 * (py-ry) / ry, 2.0);
  return (sqx + sqy <= 1) ? 1 : 0;
}

int diamond_mask(int x, int y, int w, int h, int dir) {
  return 1;
}

int thresh(int mean) {
  return mean;
}
