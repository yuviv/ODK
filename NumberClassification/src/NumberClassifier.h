#include <dirent.h>
#include <vector>
#include <string>

class NumberClassifier {
  protected:
    std::string t_dir;
    std::string c_dir;
    struct dirent **t_list;
    struct dirent **c_list;
    int t_num;
    int c_num;
    std::vector<int> guesses;
    std::vector<int> correct;
    NumberClassifier (const std::string classify_dir) : c_dir(classify_dir), 
                                          guesses(10, 0), 
                                          correct(10, 0)  {}
  public:
    virtual void classify_data(void) =0;
    virtual void print_results(void) =0;
};
