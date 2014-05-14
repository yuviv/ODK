#include <iostream>
#include "NonMLClassifier.h"

int main(int argc, char *argv[]) {
  std::cout << "Creating NonMLClassifier" << std::endl;
  NonMLClassifier classifier("./natural_numbers", 5, 4);
}
