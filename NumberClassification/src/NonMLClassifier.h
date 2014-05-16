#ifndef _NON_ML_CLASSIFIER_H_
#define _NON_ML_CLASSIFIER_H_

#include "NumberClassifier.h"

class NonMLClassifier: public NumberClassifier {
  protected:
    void process_segment(int x, int y, int w, int h);
  public:
    NonMLClassifier(const std::string classify_dir, filter_func ff, mask_func mf): NumberClassifier(classify_dir, ff, mf) {}
    void classify_data(void);
    void print_results(void);
};

#endif //_NON_ML_CLASSIFIER_H_
