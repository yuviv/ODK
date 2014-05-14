#include "NumberClassifier.h"

class NonMLClassifier: public NumberClassifier {
  private:
    int mask_w;
    int mask_h;
  public:
    NonMLClassifier(const std::string classify_dir, int w, int h): NumberClassifier(classify_dir),
                                                        mask_w(w), mask_h(h) {}
    void classify_data(void);
    void print_results(void);    
};
