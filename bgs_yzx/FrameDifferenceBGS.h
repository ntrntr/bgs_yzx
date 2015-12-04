#pragma once
#include "IBGS.h"
#include <iostream>
#include <opencv2/opencv.hpp>
class FrameDifferenceBGS :
	public IBGS
{
public:
	FrameDifferenceBGS();
	~FrameDifferenceBGS();

	
private:
	bool firstTime;
	cv::Mat img_input_prev;
	cv::Mat img_foreground;
	bool enableThreshold;
	int threshold;
	bool showOutput;
	
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
};

