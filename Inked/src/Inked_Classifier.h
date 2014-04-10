#ifndef INKED_CLASSIFIER_H
#define INKED_CLASSIFIER_H
//#include "configuration.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
//#include "Addons.h"
//#include <json/json.h>

/*
 * Inked_Classifier.h
 *
 * This class does classification on whether an image is "inked"
 * 	--whether it contains a certain threshold of ink.
 *
 *  Created on: Jan 29, 2014
 *      Author: Vivian
 */
class Inked_Classifier
{
	private:
		cv::Ptr<CvSVM> statClassifier;

		std::vector<std::string> classifications;
	//	Json::Value classifier_params;

		cv::PCA my_PCA;

		cv::Mat cMask;

		int getClassificationIdx(const std::string& filepath);

		void PCA_set_push_back(cv::Mat& PCA_set, const cv::Mat& img);
		void PCA_set_add(cv::Mat& PCA_set, std::vector<int>& trainingBubbleValues,
				const std::string& filename, bool flipExamples);
	public:
		cv::Size exampleSize;

		Inked_Classifier(): statClassifier(new CvSVM){}

		bool train_classifier( const std::vector<std::string>& examplePaths,
				cv::Size myExampleSize,
				int eigenvalues = 7,
				bool flipExamples = false);

		//void set_alignment_radius(int radius);
		//params are currently not saved
	//	void set_classifier_params(const Json::Value& classifier_params_arg);

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

#endif


