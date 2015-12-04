#pragma once
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;
class CSigmaDelta
{
public:
	CSigmaDelta();
	~CSigmaDelta();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask);
	void init(cv::Mat& image);
	void getBackgroundModel(cv::Mat& image);
	int getsgn(int a, int b);
public:
	cv::Mat m_Mean;
	cv::Mat m_Difference;
	cv::Mat m_Variance;
	cv::Mat m_foreground;
//	cv::Mat m_backgrouond;
//	cv::Mat pFrame;
	bool firsttime;
	int cols;
	int rows;
	int channals;
	int N;
};

