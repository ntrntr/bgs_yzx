#pragma once
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <iostream>
#include <cstring>
#include <map>
#include <omp.h>
const int IWonder_MODEL_SIZE = 20;
class CIWonder
{
public:
	CIWonder();
	~CIWonder();
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void getBackgroundImage(cv::Mat& image);
	void init();
	void initBackgroundModels();
	int random(int a, int b);
	void updateR();
	void modifyMask(Mat& mask);
public:
	Mat backgroundModels[IWonder_MODEL_SIZE];
	Mat backgroundModelsDownSample[IWonder_MODEL_SIZE];
	cv::Size frameSize;
	cv::Size frameSizeDownSample;
	Mat pFrame;
	Mat pFrameDownSample;
	Mat R;
	Mat RDownSample;
	Mat mask;
	Mat maskDownSample;
	Mat maskTmp;
	bool firstTime;
	int step;
	int patchsize;
	bool needRebuiltModel;
	double RThreshold;
	int m_min;
	int ts;
	int blinkTs;
	Mat pBlink;
	Mat pBlinkDownSample;
	double incR;
	double decR;
	double beta;
};

