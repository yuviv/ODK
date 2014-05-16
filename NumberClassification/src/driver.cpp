#include <iostream>
#include "driver.h"
#include "NonMLClassifier.h"
#include <cstring>

int main(int argc, char *argv[]) {
  std::cout << "Creating NonMLClassifier" << std::endl;
  NonMLClassifier classifier("./natural_numbers", &filter);
}

int filter(const struct dirent *ent) {
  const char *file_name = ent->d_name;
  const char *jpeg = ".jpg";
  return !!strstr(file_name, jpeg);
}
