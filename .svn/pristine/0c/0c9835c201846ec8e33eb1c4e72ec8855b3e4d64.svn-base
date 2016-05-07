#pragma once
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
//#define  cvConvert(src, dst) cvConvertScale((src), (dst), 1.0)
const int MODEL_SIZE2 = 20;
using namespace std;
using namespace cv;
class CAdpativeyzxVibe
{
public:
	CAdpativeyzxVibe();
	~CAdpativeyzxVibe();
	int random(int a, int b);
	void modifyMask(cv::Mat& mask);
	void initBackgroundModel();
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void saveBackgroundModels(cv::Mat& image);
public:
	double alpha;
private:
	Mat pFrame;
	Mat simple[MODEL_SIZE2];
	map<int, double> AlphaLUT;
	const int N;
	const double R;
	const int _min;
	const int ts;
	const int patchsize;
	vector<int> c_xoff;
	vector<int> c_yoff;
	bool firstFrame;
	int height, width, step, chanels;
	double beta;
	bool needRebuiltModel;
	int fgcount;
	uchar* data;
	uchar* simdata[MODEL_SIZE2];
	uchar* sg;
	
};

