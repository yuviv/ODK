#include "VivNumberClassification.h"
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
#include "PCA_SVM_Classifier.h"

using namespace cv;
using namespace std;

int filter(const struct dirent *ent) {
  const char *file_name = ent->d_name;
  const char *jpeg = ".jpg";
  return !!strstr(file_name, jpeg);
}

int windowsScandir(const char* dir, dirent ***list) {
	int n = 0;

	DIR * base = NULL;
	base = opendir(dir);
	if (base == NULL)
	{
		printf("ERROR: Unable to open directory %s\n", dir);
		exit(110);
	}

	struct dirent * de_base = readdir(base);
	while (de_base != NULL && n < 239)
	{
		char * base_name = (*de_base).d_name;

		// ignore "." and ".."
		if ((strcmp(base_name, ".") != 0) && (strcmp(base_name, "..") != 0))
		{

			*list[n] = (struct dirent *) malloc(sizeof(struct dirent));;
			/**list[n]->d_ino = de_base->d_ino;
			*list[n]->d_reclen = de_base->d_reclen;
			*list[n]->d_namlen = de_base->d_namlen;
			*list[n]->d_type = de_base->d_type;
			copy(de_base->d_name, list[n]->d_name, list[n]->d_namlen + 1);*/
			*list[n] = de_base;
			cout << de_base->d_name << endl;
			n++;
		}
		de_base = readdir(base);
	}

	closedir(base);

	return n;
}

void PCA_SVM_Classify() {
	vector<string> filepaths;
	ifstream train_set;
	train_set.open("train.txt");
	if (train_set.is_open()) {
		string line;
		int total = 0;
		while(getline(train_set, line)) {
			filepaths.push_back(string("numbers\\") + line);
			total++;
		}
		cout << "Training on " << total << " files" << endl;
	}

	Ptr<PCA_SVM_Classifier> classifier = Ptr<PCA_SVM_Classifier>(new PCA_SVM_Classifier);
	Size exampleSize = Size(20, 30);
	int eigenValue = 9;	//should this be number of eigenvectors? argc - 1? use 13?
	bool success = classifier->train_classifier(filepaths, exampleSize, eigenValue, false);

	if (success) {
		cout << "successfully trained" << endl;

		int *counts = new int[10];
		int *correct = new int[10];
		for (int i = 0; i < 10; i ++) {
			counts[i] = correct[i] = 0;
		}
		cout << "Test set:" << endl;
		ifstream test_set;
		test_set.open("test.txt");
		if (test_set.is_open()) {
			string line;
			int total = 0;
			int passed = 0;
			while(getline(test_set, line)) {
				//print out expected classification
				cout << line << "-- ";

				Mat testItem = imread("numbers\\" + line);
				Mat gray_img;
				cvtColor(testItem, gray_img, CV_BGR2GRAY);

				string expected_class = line.substr(0, line.find_first_of("_"));
				cout << "expected: " << expected_class << ", ";
				int num = expected_class.at(0) - 0x30;
				total++;

				Point location(10, 15);
				string actual_class = classifier->classify_item(gray_img, location);
				cout << "actual: " << actual_class << ", ";

				counts[num]++;
				if (expected_class.compare(actual_class) == 0) {
					cout << "PASS!" << endl;
					passed++;
					correct[num]++;
				} else {
					cout << "FAIL" << endl;
				}
			}
			test_set.close();

			for (int i = 0; i < 10; i++) {
				cout << "Correctly guessed " << correct[i] << " out of " << counts[i] << " instances of " << i << endl;
			}

			double perc_correct = 100.0 * passed / total;
			cout << "Correct " << passed << " out of " << total << endl;
			cout << perc_correct << "% correct" << endl;
		} else {
			cout << "unable to open file";
		}
	} else {
		cout << "training failed" << endl;
	}

}

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
    case FOUR_SEGMENTS_ERR:
      return 4;
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
	//PCA_SVM classifier
  PCA_SVM_Classify();
/*
	vector<string> filePaths;
	CrawlFileTree("numbers", filePaths);

	// Matt's classifier code:

  int n = filePaths.size();
  n -= 2;
  cout << "size is " << n << endl;
  int *guesses = new int [10]();
  int *correct = new int [10]();
  //struct dirent **list;

  //n = scandir("./numbers", &list, filter, alphasort);
  if (n < 0) {
    cerr << "ERROR: Bad directory" << endl;
  } else {
    while (n--) {
      //string img_name(list[n]->d_name);
    	string img_name = filePaths[n];
      int img_num = img_name.at(8)- 0x30;
      char guess = 0x0;
      cout << "Processing file for #" << img_num << ": " << img_name << endl;
      Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR); //imread(string("./numbers/") + img_name, CV_LOAD_IMAGE_COLOR);
      if (img.empty()) {
        cerr << "ERROR: could not read image " << img_name << endl;
      }
      Mat img_gray(img.size(), CV_8U);
      cvtColor(img, img_gray, CV_BGR2GRAY);
      Mat img_bin(img_gray.size(), img_gray.type());
      threshold(img_gray, img_bin, 160, 255, THRESH_BINARY);
      resize(img_bin, img_bin, Size(20, 30), 0, 0, INTER_AREA);
      checkSegment(guess, TOP, img_bin, 7, 2, 6, 6, img_name);
      checkSegment(guess, MIDDLE, img_bin, 7, 12, 6, 6, img_name);
      checkSegment(guess, BOTTOM, img_bin, 7, 22, 6, 6, img_name);
      checkSegment(guess, TOP_LEFT, img_bin, 2, 7, 6, 6, img_name);
      checkSegment(guess, TOP_RIGHT, img_bin, 12, 7, 6, 6, img_name);
      checkSegment(guess, BOTTOM_LEFT, img_bin, 2, 19, 6, 6, img_name);
      checkSegment(guess, BOTTOM_RIGHT, img_bin, 12, 19, 6, 6, img_name);
      int n = predictNumber(guess);
      guesses[img_num]++;
      if (n < 0) {
        cout << "Could not predict number" << endl;
      } else {
        cout << "Predicted: " << n << endl;
        if (n == img_num)
          correct[n]++;
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    cout << "Correctly guessed " << correct[i] << " out of " << guesses[i] << " instances of " << i << endl;
  }

  delete [] guesses;
  delete [] correct;


	return 0;
}*/
