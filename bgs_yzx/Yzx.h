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
struct Yzx
{
	vector<Mat> backgroundModel;
	vector<Mat> frequency;
	Mat minBrightness;
	Mat maxBrightness;
	Mat T;
	Mat meanD;
	Mat R;
};
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
	uchar getNeighbor(int x, int y);
	void updateBackgroundModel(int x, int y);
	void updateNeighborModel(int x, int y);
	void updateT(int flag, float* pt, float* pmean);
	void updateR(float* pr, float* pmean);
private:
	const int N;
	double alpha;
	bool firstTime;
	Mat pFrame;
	cv::RNG rng;
	Yzx BGModel;
	int rows;
	int cols;
	vector<float*> pBM;
	vector<int*> pFrequency;
	int m_min;
	Mat foreground;
	int meanLearningN;
	int meanDCount;
	float T_dec;
	float T_inc;
	float T_lower;
	float T_upper;
	float R_inc_dec;
	float R_lower;
	float R_scale;
};

