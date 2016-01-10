#pragma once
#include "imbs_org.hpp"
class CIMBS
{
public:
	CIMBS();
	~CIMBS();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void getBackgroundImage(cv::Mat& image);
	void init();
public:
	BackgroundSubtractorIMBS* pIMBS;
};

