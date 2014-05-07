#include "NumberClassification.h"
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <opencv/cvwimage.h>
#include <vector>
#include <cstring>
#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>

using namespace cv;
using namespace std;
using namespace tesseract;

const char* SEGMENT_NAMES[] = { "top", "middle", "bottom",
                                "top_left", "top_right",
                                "bottom_left", "bottom_right"};

int filter(const struct dirent *ent) {
  const char *file_name = ent->d_name;
  const char *jpeg = ".jpg";
  return !!strstr(file_name, jpeg);
}

void checkSegment(char& guess, const char& segment, Mat img, int x, int y, int width, int height, string img_name) {
  Rect r(x, y, width, height);
  Mat cropped = img(r);
  if (TOTAL_PIXELS - countNonZero(cropped) > PIXEL_THRESHOLD)
    guess |= segment;
  
  if (segment == TOP) {
    imwrite(string("./segments/top/") + img_name, cropped);
  } else if (segment == MIDDLE) {
    imwrite(string("./segments/middle/") + img_name, cropped);
  } else if (segment == BOTTOM) {
    imwrite(string("./segments/bottom/") + img_name, cropped);
  } else if (segment == TOP_LEFT) {
    imwrite(string("./segments/top_left/") + img_name, cropped);
  } else if (segment == TOP_RIGHT) {
    imwrite(string("./segments/top_right/") + img_name, cropped);
  } else if (segment == BOTTOM_LEFT) {
    imwrite(string("./segments/bottom_left/") + img_name, cropped);
  } else {
    imwrite(string("./segments/bottom_right/") + img_name, cropped);
  }
}

int predictNumber(const char& guess) {
  //printf("Guess: %x\n", guess);
  switch (guess) {
    case EIGHT_SEGMENTS:
      return 8;
    case ZERO_SEGMENTS:
      return 0;
    case NINE_SEGMENTS:
      return 9;
    case NINE_SEGMENTS_PARTIAL:
      return 9;
    case SIX_SEGMENTS:
      return 6;
    case SIX_SEGMENTS_PARTIAL:
      return 6;
    case THREE_SEGMENTS:
      return 3;
    case TWO_SEGMENTS:
      return 2;
    case FIVE_SEGMENTS:
      return 5;
    case FOUR_SEGMENTS:
      return 4;
    case SEVEN_SEGMENTS:
      return 7;
    case ONE_SEGMENTS_LEFT:
      return 1;
    case ONE_SEGMENTS_RIGHT:
      return 1;
    default:
      return -1;
  }
}

int main(int argc, char** argv) {
  int n;
  int *guesses = new int [10];
  int *correct = new int [10];
  struct dirent **list;
 
  cout << "Classifying using pixel thresholds (no training)" << endl;

  n = scandir("./numbers", &list, filter, alphasort);
  if (n < 0) {
    cerr << "ERROR: Bad directory" << endl;
  } else {
    while (n--) {
      string img_name(list[n]->d_name);
      int img_num = img_name.at(0) - 0x30;
      char guess = 0x0;
      //cout << "Processing file for #" << img_num << ": " << img_name << endl;
      Mat img = imread(string("./numbers/") + img_name, CV_LOAD_IMAGE_COLOR);
      if (img.empty()) {
        cerr << "ERROR: could not read image " << img_name << endl;
      }
      Mat img_gray(img.size(), CV_8U);
      cvtColor(img, img_gray, CV_BGR2GRAY);
      resize(img_gray, img_gray, Size(20, 30), 0, 0, INTER_AREA);
      vector<Mat> channels;
      split(img, channels);
      int thresh = (int)mean(channels[0])[0];    
      Mat img_bin(img_gray.size(), img_gray.type());
      threshold(img_gray, img_bin, thresh, 255, THRESH_BINARY);
      imwrite(string("./bin/") + img_name, img_bin);
      /*bitwise_not(img_bin, img_bin);
      TessBaseAPI tess;
      tess.Init(NULL, "eng", OEM_DEFAULT);
      tess.SetPageSegMode(PSM_SINGLE_BLOCK);
      tess.SetImage((uchar*)img_bin.data, img_bin.cols, img_bin.rows, 1, img_bin.cols);
      tess.SetVariable("tessedit_char_whitelist", "0123456789");
      
      char* out = tess.GetUTF8Text();
      cout << out << endl;
    }
  }*/
      checkSegment(guess, TOP, img_bin, 9, 1, 2, 8, img_name);
      checkSegment(guess, MIDDLE, img_bin, 9, 11, 2, 8, img_name);
      checkSegment(guess, BOTTOM, img_bin, 9, 22, 2, 8, img_name);
      checkSegment(guess, TOP_LEFT, img_bin, 1, 9, 8, 2, img_name);
      checkSegment(guess, TOP_RIGHT, img_bin, 11, 9, 8, 2, img_name);
      checkSegment(guess, BOTTOM_LEFT, img_bin, 1, 20, 8, 2, img_name);
      checkSegment(guess, BOTTOM_RIGHT, img_bin, 11, 20, 8, 2, img_name);
      int n = predictNumber(guess);      
      guesses[img_num]++;
      if (n < 0) {
        cout << "Could not predict number for " << img_name << endl;
        cout << "Guessed: " << (int)guess << endl;
      } else {
        //cout << "Predicted: " << n << endl;
        if (n == img_num)
          correct[n]++;
        else {
          cout << "Could not predict number for " << img_name << endl;
          cout << "Guessed: " << (int)guess << endl;
        }
      }
    }
  }
  
  int total_correct = 0;
  int total_guesses = 0;
  for (int i = 0; i < 10; i++) {
    cout << "Correctly guessed " << correct[i] << " out of " << guesses[i] << " instances of " << i << endl;
    total_correct += correct[i];
    total_guesses += guesses[i];
  }

  cout << "Total guessed " << total_correct << " out of " << total_guesses << endl;


  delete [] guesses;
  delete [] correct;

	return 0;
}
