/*
 * CheckboxPCA.h
 *
 *  Created on: May 19, 2014
 *      Author: Vivian
 */

#ifndef CHECKBOXPCA_H_
#define CHECKBOXPCA_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

/*
This class does classification using OpenCV's support vector machine and PCA.
*/
class CheckboxPCA
{
	private:
		cv::Ptr<CvSVM> statClassifier;

		std::vector<std::string> classifications;

		cv::PCA my_PCA;

		cv::Mat cMask;

		int getClassificationIdx(const std::string& filepath);

		void PCA_set_push_back(cv::Mat& PCA_set, const cv::Mat& img);
		void PCA_set_add(cv::Mat& PCA_set, int x, int y, std::vector<int>& trainingBubbleValues,
		                 const std::string& filename, bool flipExamples);
	public:
		cv::Size exampleSize;

		CheckboxPCA(): statClassifier(new CvSVM){}

		bool train_classifier( const std::vector<std::string>& examplePaths,
		                           cv::Size myExampleSize,
		                           int x = 0,
		                           int y = 0,
		                           int eigenvalues = 7,
		                           bool flipExamples = false);

		//Given a image and location in that image, this rates how similar it is to the training examples
		//Lower score = more similar
		double rate_item(const cv::Mat& det_img_gray, const cv::Point& item_location) const;
		//Returns a refined location for the object being classified
		//(currently by doing a hill climbing search with rateBubble as the objective function)
		cv::Point align_item(const cv::Mat& det_img_gray, const cv::Point& seed_location, double alignment_radius) const;

		std::string classify_item(const cv::Mat& det_img_gray, const cv::Point& item_location) const;

		void save(const std::string& outputPath) const throw(cv::Exception);
		void load(const std::string& inputPath) throw(cv::Exception);
};



#endif /* CHECKBOXPCA_H_ */
