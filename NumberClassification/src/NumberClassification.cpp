//============================================================================
// Name        : NumberClassification.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include "Inked_Classifier.h"
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <opencv/cvwimage.h>
//#include <opencv2/imgproc/imgproc_c.h>
#include <vector>

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

int main(int argc, char** argv) {
	/*cout << "Training set: " << endl;

	vector<string> filepaths;
	for(int i = 1; i < argc; i++) {
		stringstream ss;
		ss << "bubbles\\" << argv[i];
		filepaths.push_back(ss.str());
		cout << ss.str() << " ";
	}
	cout << endl;

	cout << "Begin training: ";*/

	Mat img = imread("numbers\\0_190.jpg", 1 );
	cvtColor(img, img, CV_BGR2GRAY);	//change to gray scale?
	resize(img, img, Size(20, 30), 0, 0, INTER_AREA);
	if (!img.data)
		cout << "no image found" << endl;

	//namedWindow( "test", CV_WINDOW_AUTOSIZE );
	imshow( "test", img );
	cout << "img dimensions " << img.cols << ", " << img.rows << endl;

	/*int x[] = {5, 5, 5, 0, 10, 0, 10};
	int y[] = {0, 10, 20, 5, 5, 15, 15};*/ //for square points

	//Diamond points: (10, 0) (5, 5) (15, 5) (10, 10)
	Point2f p1(10.0f, 0.0f);
	Point2f p2(5.0f, 5.0f);
	Point2f p3(15.0f, 5.0f);
	Point2f p4(10.0f, 10.0f);
	Point2f x[] = {p1, p2, p3, p4};
	vector<Point2f> pts(x, x + sizeof x / sizeof x[0]);

	for (int i = 0; i < 1; i++) {
		stringstream ss;
		ss << "cropped\\0_190_" << i << ".jpg";
		Mat cropped;
		/*Rect r(x[i], y[i], 10, 10);	//TODO: crop diamonds?
		cropped = img(r);*/

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
