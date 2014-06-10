//============================================================================
// Name        : BubbleClassifier.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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
#include <algorithm>
#include "BubbleClassifier.h"
#include "BubblePCA.h"

using namespace cv;
using namespace std;

#define USE_PCA true

static void HandleDir(char *dirpath, DIR *d, vector<string> &filenames) {
  // Loop through the directory.
  while (1) {
    char *newfile;
    int res, charsize;
    struct stat nextstat;
    struct dirent *dirent = NULL;

    // Use the "readdir()" system call to read the next directory
    // entry. (man 3 readdir).  If we hit the end of the
    // directory (i.e., readdir returns NULL and errno == 0),
    // return back out of this function.  Note that you have
    // access to the errno global variable as a side-effect of
    // us having #include'd <errno.h>.
    errno = 0;

    dirent = readdir(d);
    if (dirent == NULL && errno == 0) {
    	return;
    }

    // If the directory entry is named "." or "..",
    // ignore it.  (use the C "continue;" expression
    // to begin the next iteration of the while() loop.)
    // You can find out the name of the directory entry
    // through the "d_name" field of the struct dirent
    // returned by readdir(), and you can use strcmp()
    // to compare it to "." or ".."
    if ((strcmp(dirent->d_name, ".") == 0) ||
        (strcmp(dirent->d_name, "..") == 0))
      continue;

    // We need to append the name of the file to the name
    // of the directory we're in to get the full filename.
    // So, we'll malloc space for:
    //
    //     dirpath + "/" + dirent->d_name + '\0'
    charsize = strlen(dirpath) + 1 + strlen(dirent->d_name) + 1;
    newfile = (char *) malloc(charsize);
    assert(newfile != NULL);
    if (dirpath[strlen(dirpath)-1] == '/') {
      // no need to add an additional '/'
      snprintf(newfile, charsize, "%s%s", dirpath, dirent->d_name);
    } else {
      // we do need to add an additional '/'
      snprintf(newfile, charsize, "%s/%s", dirpath, dirent->d_name);
    }

    // Use the "lstat()" system call to ask the operating system
    // to give us information about the file named by the
    // directory entry.   ("man lstat")
    res = stat(newfile, &nextstat);
    if (res == 0) {
      // Test to see if the file is a "regular file" using
      // the S_ISREG() macro described in the lstat man page.
      // If so, process the file by invoking the HandleFile()
      // private helper function.
      //
      // On the other hand, if the file turns out to be a
      // directory (which you can find out using the S_ISDIR()
      // macro described on the same page, then you need to
      // open the directory using opendir()  (man 3 opendir)
      // and recursively invoke HandleDir to handle it.
      // Be sure to call the "closedir()" system call
      // when the recursive HandleDir() returns to close the
      // opened directory.

      if (S_ISREG(nextstat.st_mode))
        filenames.push_back(newfile);

      if (S_ISDIR(nextstat.st_mode)) {
        DIR* d = opendir(newfile);
        HandleDir(newfile, d, filenames);
        closedir(d);
      }
    }

    // Done with this file.  Fall back up to the next
    // iteration of the while() loop.
    free(newfile);
  }
}

int CrawlFileTree(char* rootdir, vector<string> &filenames) {
	struct stat rootstat;
	int result;
	DIR *rd;


	// Verify that rootdir is a directory.
	/*result = lstat((char *) rootdir, &rootstat);
	if (result == -1) {
		// We got some kind of error stat'ing the file. Give up
		// and return an error.
		return 0;
	}
	if (!S_ISDIR(rootstat.st_mode) && !S_ISLNK(rootstat.st_mode)) {
		// It isn't a directory, so give up.
		return 0;
	}*/

	// Try to open the directory using opendir().  If try but fail,
	// (e.g., we don't have permissions on the directory), return NULL.
	// ("man 3 opendir")
	rd = opendir(rootdir);
	if (rd == NULL) {
		return 0;
	}

	// Begin the recursive handling of the directory.
	HandleDir(rootdir, rd, filenames);

	// All done, free up.
	assert(closedir(rd) == 0);
	return 1;
}

// Fills the output vector filenames with the all the files in rootdir
// Returns 1 on success, 0 on failure
int CrawlFileTree(string rootdir, vector<string> &filenames) {
	vector<char> writable(rootdir.begin(), rootdir.end());
	writable.push_back('\0');
	return CrawlFileTree(&writable[0], filenames);
}

// Train the PCA_SVM classifier on the listed training images in trainFile (.txt)
bool trainPCA(const char *trainFile, Ptr<BubblePCA> classifier) {
	vector<string> filepaths;
	ifstream train_set;
	train_set.open(trainFile);
	if (train_set.is_open()) {
		string line;
		int total = 0;
		while(getline(train_set, line)) {
			filepaths.push_back(line);
			total++;
		}
		cout << "Training on " << total << " files" << endl;
	}

	Size exampleSize = Size(30, 30);
	int eigenValue = filepaths.size() - 1;
	int x = 5;
	int y = 5;
	bool success = classifier->train_classifier(filepaths, exampleSize, x, y, eigenValue, false);
	cout << "training finished" << endl;
	return success;
}

// Predict this images classification using pixel thresholding
string thresholdPredict(Mat &img_gray) {
	Mat img_bin(img_gray.size(), img_gray.type());
	threshold(img_gray, img_bin, 160, 255, THRESH_BINARY);
	int pixels = countNonZero(img_bin);
	if (pixels < PIXEL_THRESHOLD) {
		return "filled";
	} else {
		return "empty";
	}
}

int main(int argc, char **argv) {
	// Get file paths of images to classify
	vector<string> filePaths;
	string rootDir = "filled/more";
	if (argc == 2)
		rootDir = string(argv[1]);
	int res = CrawlFileTree("filled/f1", filePaths);
	if (res == 0) {
		cerr << "ERROR: bad directory" << endl;
	}

	int n = filePaths.size() - 1;
  int count = 0;
  int correct = 0;

  // Train classifier
  Ptr<BubblePCA> pca = Ptr<BubblePCA>(new BubblePCA);
  if (USE_PCA) {
  	cout << "USING PCA CLASSIFICATION" << endl;
		bool trained = trainPCA("training.txt", pca);
		if (!trained)
			cerr << "TRAINING DIDN'T WORK" << endl;
  } else {
  	cout << "USING THRESHOLDING CLASSIFICATION" << endl;
  }

  cout << "FAILED SAMPLES:" << endl;
	while (n--) {
		string img_name = filePaths[n];
		string classification = img_name.substr(0, img_name.find_first_of("/"));
		Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
		if (img.empty()) {
			cerr << "ERROR: could not read image " << img_name << endl;
		}
		count++;
		Mat img_gray(img.size(), CV_8U);
		cvtColor(img, img_gray, CV_BGR2GRAY);

		// Predict classification
		string predicted_class;
		if (USE_PCA) {
			// PCA
			Rect r(5, 5, 30, 30);
			img_gray = img_gray(r);
			Point loc(15, 15);
			predicted_class = pca->classify_item(img_gray, loc);
		} else {
			// THRESHOLDING
			Rect r(15, 15, 10, 10);
			img_gray = img_gray(r);
			predicted_class = thresholdPredict(img_gray);
		}

		// Update correct and print out failed samples
		if (predicted_class.compare(classification) == 0) {
			correct++;
		} else {
			cout << img_name << endl;
			cout << "  Predicted " << predicted_class;
			cout << ", Should be " << classification << endl;
		}
	}

	cout << "predicted " << correct << " correct out of " << count << " samples" << endl;

	return 0;
}
