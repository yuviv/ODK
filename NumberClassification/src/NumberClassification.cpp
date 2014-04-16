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

using namespace cv;
using namespace std;

vector<string> split(string s) {
    vector<string> elems;
    int index = s.find_first_of(" ");
    while(s.size() > 0 && index >= 0) {
    	elems.push_back(s.substr(0, index));
    	s = s.substr(index + 1);
    }
    elems.push_back(s);
    return elems;
}

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
  if (segment == TOP)
    imwrite(string("./top/") + img_name, cropped);
  else if (segment == MIDDLE) {
    imwrite(string("./middle/") + img_name, cropped);
  } else if (segment == BOTTOM) {
    imwrite(string("./bottom/") + img_name, cropped);
  } else if (segment == TOP_LEFT) {
    imwrite(string("./top_left/") + img_name, cropped);
  } else if (segment == TOP_RIGHT) {
    imwrite(string("./top_right/") + img_name, cropped);
  } else if (segment == BOTTOM_LEFT) {
    imwrite(string("./bottom_left/") + img_name, cropped);
  } else {
    imwrite(string("./bottom_right/") + img_name, cropped);
  }
}

int main(int argc, char** argv) {
  int n;
  struct dirent **list;
  
  n = scandir("./numbers", &list, filter, alphasort);
  if (n < 0) {
    cerr << "ERROR: Bad directory" << endl;
  } else {
    while (n--) {
      string img_name(list[n]->d_name);
      int img_num = img_name.at(0) - 0x30;
      char guess = 0x0;
      cout << "Processing file for #" << img_num << ": " << img_name << endl;
      Mat img = imread(string("./numbers/") + img_name, CV_LOAD_IMAGE_COLOR);
      if (img.empty()) {
        cerr << "ERROR: could not read image " << img_name << endl;
      }
      Mat img_gray(img.size(), CV_8U);
      cvtColor(img, img_gray, CV_BGR2GRAY);
      Mat img_bin(img_gray.size(), img_gray.type());
      threshold(img_gray, img_bin, 130, 255, THRESH_BINARY);
      resize(img_bin, img_bin, Size(20, 30), 0, 0, INTER_AREA);
      cout <<"Image dimensions: (" << img.rows << ", " << img.cols << ")" << endl;
      Mat cropped;
      checkSegment(guess, TOP, img_bin, 2, 2, 15, 6, img_name);
      checkSegment(guess, MIDDLE, img_bin, 2, 12, 15, 6, img_name);
      checkSegment(guess, BOTTOM, img_bin, 2, 22, 15, 6, img_name);
      checkSegment(guess, TOP_LEFT, img_bin, 2, 2, 6, 15, img_name);
      checkSegment(guess, TOP_RIGHT, img_bin, 12, 2, 6, 15, img_name);
      checkSegment(guess, BOTTOM_LEFT, img_bin, 2, 13, 6, 15, img_name);
      checkSegment(guess, BOTTOM_RIGHT, img_bin, 12, 13, 6, 15, img_name);
      vector<int> compression_params;
      compression_params.push_back(CV_IMWRITE_PXM_BINARY);
      imwrite(string("./bin/") + img_name, img_bin, compression_params);
      printf("Segments: %x\n", guess);
    }
  }

	/*int x[] = {5, 5, 5, 0, 10, 0, 10};
	int y[] = {0, 10, 20, 5, 5, 15, 15};*/ //for square points

	//Diamond points: (10, 0) (5, 5) (15, 5) (10, 10)
	/*Point2f p1(10.0f, 0.0f);
	Point2f p2(5.0f, 5.0f);
	Point2f p3(15.0f, 5.0f);
	Point2f p4(10.0f, 10.0f);
	Point2f x[] = {p1, p2, p3, p4};
	vector<Point2f> pts(x, x + sizeof x / sizeof x[0]);
*/
	/*for (int i = 0; i < 1; i++) {
		stringstream ss;
		ss << "cropped\\0_190_" << i << ".jpg";
		Mat cropped;
		//Rect r(x[i], y[i], 10, 10);	//TODO: crop diamonds?
		cropped = img(r);

		Mat pts;
		//Get rectangle from set of points
		RotatedRect rdt(Point2f(10, 10), Size2f(7.07, 7.07), 45);//minAreaRect(pts);

		//Calculate rotation matrix
		Scalar sc(0);
		Mat M = getRotationMatrix2D(rdt.center, rdt.angle, 1);

		//Perform rotation
		warpAffine(img, cropped, M, img.size(), INTER_LINEAR, BORDER_CONSTANT, sc);
		//image = clusterImage;

		cout << ss.str() << ", ";
		imwrite(ss.str(), cropped);

	}

	waitKey(0);
*/
	/*
	Ptr<Inked_Classifier> classifier = Ptr<Inked_Classifier>(new Inked_Classifier);
	Size exampleSize = Size(50, 50);
	int eigenValue = argc - 1;	//should this be number of eigenvectors? argc - 1?
	bool flipExamples = false;

	bool success = classifier->train_classifier(filepaths, exampleSize, eigenValue, flipExamples);
	if (success) {
		cout << "successfully trained" << endl;

		cout << "Test set:" << endl;
		ifstream test_set;
		test_set.open("test_set.txt");
		if (test_set.is_open()) {
			string line;
			int total = 0;
			int passed = 0;
			while(getline(test_set, line)) {
				//print out expected classification
				int nameIdx = line.find_last_of("\\");
				string filename = line.substr(nameIdx + 1, line.size() - nameIdx);
				string expected_class = filename.substr(0, filename.find_first_of("_"));
				cout << "expected: " << expected_class << ", ";
				total++;

				Mat testItem = imread("bubbles\\" + line);
				Mat gray_img;
				cvtColor(testItem, gray_img, CV_BGR2GRAY);
				Point location(25,25);
				string actual_class = classifier->classify_item(gray_img, location);
				cout << "actual: " << actual_class << ", ";

				if (expected_class.compare(actual_class) == 0) {
					cout << "PASS!" << endl;
					passed++;
				} else {
					cout << "FAIL" << endl;
				}
			}
			test_set.close();
			double perc_correct = 100.0* passed / total;
			cout << "Correct " << passed << " out of " << total << endl;
			cout << perc_correct << "% correct" << endl;
		} else {
			cout << "unable to open file";
		}
	} else {
		cout << "training failed" << endl;
	}
	*/

	return 0;
}
