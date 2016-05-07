#pragma once
class CLSD
{
public:
	CLSD();
	~CLSD();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void saveBackgroundModels(cv::Mat& image);
};

