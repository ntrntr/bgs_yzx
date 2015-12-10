#pragma once
#include "IBGS.h"
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <time.h>
#include <iostream>
#include <cstring>
#include <map>
#include <omp.h>
class CYzx
{
public:
	CYzx();
	~CYzx();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask);
	void getBackgroundModel(cv::Mat& image);
	void initBackground();
	void init();
	void modifyMask(cv::Mat& mask);
private:
	double alpha;
	bool firstTime;
};

