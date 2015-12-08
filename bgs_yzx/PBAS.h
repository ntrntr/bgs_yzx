#pragma once
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include <time.h>
#include <iostream>
#include <cstring>
#include <map>
#include <list>
using namespace std;
using namespace cv;
const int N = 35;
class CPBAS
{
public:
	CPBAS();
	~CPBAS();
public:
	void operator()(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void init();
	void initBackgroundModel();
	int getX(int x);
	int getY(int y);
	int random(int a, int b);
	void getBackgroundModel(cv::Mat &image);
private:

	int m_min;
	float R_inc_dec;
	float R_lower;
	int R_scale;
	float T_dec;
	float T_inc;
	float T_lower;
	float T_upper;
	float alpha;
	//vector<Mat> backgroundModel;
	//vector<Mat> R;
	//vector<Mat> D;
	//Mat backgroundModel[N];
	vector<vector<uchar>> backgroundModel;
	vector<list<double>> D;
	//float* pBGM[N];
	Mat R;
	float* pR;
	//Mat D[N];
	//float* pD[N];
	Mat T;
	float* pT;
	Mat background;
	bool firstTime;
	int patchsize;
	vector<int> c_xoff;
	vector<int> c_yoff;
	Mat pFrame;
	Mat foregroundModel;
	uchar* pFGM;
	
	int height, width, stepUchar, channels;
	int stepFloat;
};

