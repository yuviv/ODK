#include <iostream>
#include "driver.h"
#include "NonMLClassifier.h"
#include <cstring>

int main(int argc, char *argv[]) {
  std::cout << "Creating NonMLClassifier" << std::endl;
  NonMLClassifier classifier("./natural_numbers/", &filter, &rect_mask, &thresh, 20, 30, 2, 8, 2);
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

int thresh(int mean) {
  return mean;
}
