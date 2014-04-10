//============================================================================
// Name        : Inked.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Inked_Classifier.h"
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

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

    if (dirent == NULL && errno == 0) return;

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
 //   res = lstat(newfile, &nextstat); TODO: MAKE LSTAT WORK!!!
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

void cropImages(int argc, char** origFiles) {
	int i;
	for (i = 1; i < argc; i++) {
		string line = origFiles[i];
		int nameIdx = line.find_last_of("\\");
		string filename = line.substr(nameIdx + 1, line.size() - nameIdx);

		Mat testItem = imread("segments2\\" + line);
		Mat gray_img;
		cvtColor(testItem, gray_img, CV_BGR2GRAY);

		string expected_class = filename.substr(0, filename.find_first_of("_"));
		if (expected_class.compare("a") == 0) {
			Rect roi(15, 15, 25, 25);
			testItem = gray_img(roi);
		} else {
			Rect roi(5, 15, 25, 25);
			testItem = gray_img(roi);
		}
		imwrite("cropped2\\" + line, testItem);
	}
}

int main(int argc, char** argv) {

	//only need this for first run
	/*cout << "Cropping Images..." << endl;
	cropImages(argc, argv);
	cout << "complete!" << endl;*/

	cout << "Training set: " << endl;
	vector<string> filepaths;
	for(int i = 1; i < argc; i++) {
		stringstream ss;
		ss << "cropped2\\" << argv[i];
		filepaths.push_back(ss.str());
		cout << ss.str() << " ";
	}
	cout << endl;


	cout << "Begin training: ";
	//CrawlFileTree("\\bubbles\\", filepaths);


	Ptr<Inked_Classifier> classifier = Ptr<Inked_Classifier>(new Inked_Classifier);
	//classifier->load("bubbles\\cached_classifier_data_16x14.yml");
	Size exampleSize = Size(30, 30);
	int eigenValue = 9;	//should this be number of eigenvectors? argc - 1? use 13?
	bool flipExample = true;

	bool success = classifier->train_classifier(filepaths, exampleSize, eigenValue, flipExample);
	//bool success = true;
	if (success) {
		cout << "successfully trained" << endl;

		classifier->save("inked_classifier.txt");

		cout << "Test set:" << endl;
		ifstream test_set;
		test_set.open("segments1\\test.txt");
		if (test_set.is_open()) {
			string line;
			int total = 0;
			int passed = 0;
			while(getline(test_set, line)) {
				//print out expected classification
				int nameIdx = line.find_last_of("\\");
				string filename = line.substr(nameIdx + 1, line.size() - nameIdx);

				cout << line << "-- ";

				Mat testItem = imread("segments1\\" + line);
				Mat gray_img;
				cvtColor(testItem, gray_img, CV_BGR2GRAY);

				Mat cropped;
				string expected_class = filename.substr(0, filename.find_first_of("_"));
				if (expected_class.compare("a") == 0) {
					expected_class = "filled";
					Rect roi(15, 15, 25, 25);
					cropped = gray_img(roi);
				} else {
					expected_class = "empty";
					Rect roi(5, 15, 25, 25);
					cropped = gray_img(roi);
				}
				cout << "expected: " << expected_class << ", ";
				total++;

				/*double alignment_radius = 2.0; //TODO: what to use here?
				Point itemLocation(cropped.rows / 2, cropped.cols / 2);
				itemLocation = classifier->align_item(cropped, itemLocation, alignment_radius);*/

				Point location(12.5, 12.5);
				string actual_class = classifier->classify_item(cropped, location);
				if (actual_class.compare("a") == 0) {
					actual_class = "filled";
				} else {
					actual_class = "empty";
				}
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

	return 0;
}
