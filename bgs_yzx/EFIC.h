#pragma once
class CEFIC
{
public:
	CEFIC();
	~CEFIC();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void saveBackgroundModels(cv::Mat& image);
public:
	Mat pFrame;
};

