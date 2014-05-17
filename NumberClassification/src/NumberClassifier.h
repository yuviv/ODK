#ifndef _NUMBER_CLASSIFIER_H_
#define _NUMBER_CLASSIFIER_H_

#include <dirent.h>
#include <vector>
#include <string>

#include "NumberClassification.h"
#include "SegmentMask.h"

typedef int (*filter_func)(const struct dirent *ent);

class NumberClassifier {
  protected:
    struct dirent **num_list;
    int n_numbers;
    std::vector<int> guesses;
    std::vector<int> correct;
    NumberClassifier (const std::string classify_dir,
                      filter_func ff, mask_func mf) : guesses(10, 0), 
                                                      correct(10, 0) {
      n_numbers = scandir(classify_dir.c_str(), &num_list, ff, alphasort);
    }
    int predict_number(const char guess);
  public:
    virtual void classify_data(void) {};
    virtual void print_results(void) {};
};

#endif //_NUMBER_CLASSIFIER_H_
