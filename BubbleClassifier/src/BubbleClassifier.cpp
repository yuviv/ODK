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

using namespace cv;
using namespace std;

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

int CrawlFileTree(string rootdir, vector<string> &filenames) {
	vector<char> writable(rootdir.begin(), rootdir.end());
	writable.push_back('\0');
	return CrawlFileTree(&writable[0], filenames);
}

int main() {
	vector<string> filePaths;
	int res = CrawlFileTree("filled/f6", filePaths);
	if (res == 0) {
		cerr << "ERROR: bad directory" << endl;
	}

	int n = filePaths.size() - 2;
  int sumPixels = 0;
  int min = INT_MAX;
  int max = INT_MIN;

  int count = 0;
  int correct = 0;

	while (n--) {
		string img_name = filePaths[n];
		char guess = 0x0;
		cout << "Processing file for : " << img_name << endl;
		Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
		if (img.empty()) {
			cerr << "ERROR: could not read image " << img_name << endl;
		}
		Mat img_gray(img.size(), CV_8U);
		cvtColor(img, img_gray, CV_BGR2GRAY);
		Rect r(15, 15, 10, 10);
		img_gray = img_gray(r);
		Mat img_bin(img_gray.size(), img_gray.type());
		threshold(img_gray, img_bin, 160, 255, THRESH_BINARY);
    int pixels = countNonZero(img_bin);
    cout << pixels << " pixels filled" << endl;

    if (pixels < PIXEL_THRESHOLD) {
    	cout << "  FILLED" << endl;
    	correct++;
    } else {
    	cout << "  EMPTY -- FAILED" << endl;
    }
    count++;
    sumPixels += pixels;
    min = std::min(min, pixels);
    max = std::max(max, pixels);
	}

	double average = (double) sumPixels / (filePaths.size() - 2);
	cout << "average pixels: " << average << endl;
	cout << "min pixels: " << min << endl;
	cout << "max pixels: " << max << endl;

	cout << "predicted " << correct << " correct out of " << count << " samples" << endl;

	return 0;
}
