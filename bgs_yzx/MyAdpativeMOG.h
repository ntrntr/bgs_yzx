#pragma once
#include "precomp.hpp"
#include <float.h>
#include "cv.h"
#include <cxcore.h>
#include <highgui.h>
using namespace cv;
#undef K
#undef L
#undef T
//static const int defaultNMixtures = 5;
//static const int defaultHistory = 200;
//static const double defaultBackgroundRatio = 0.7;
//static const double defaultVarThreshold = 2.5*2.5;
//static const double defaultNoiseSigma = 30 * 0.5;
//static const double defaultInitialWeight = 0.05;
//template<typename VT> struct MixData
//{
//	float sortKey;
//	float weight;
//	VT mean;
//	VT var;
//};
class CMyAdpativeMOG
{
public:
	CMyAdpativeMOG();
	~CMyAdpativeMOG();
	void operator()(cv::InputArray _image, cv::OutputArray _fgmask, double learningRate);
	void getBackgroundImage(cv::Mat& image) const;
	CMyAdpativeMOG(int _history, int _nmixtures,
		double _backgroundRatio,
		double _noiseSigma);
	void initialize(cv::Size _frameSize, int _frameType);
	static void process8uC1(const cv::Mat& image, cv::Mat& fgmask, double learningRate,
		cv::Mat& bgmodel, int nmixtures, double backgroundRatio,
		double varThreshold, double noiseSigma);
	static void process8uC3(const cv::Mat& image, cv::Mat& fgmask, double learningRate,
		cv::Mat& bgmodel, int nmixtures, double backgroundRatio,
		double varThreshold, double noiseSigma);

protected:
	cv::Size frameSize;
	int frameType;
	cv::Mat bgmodel;
	int nframes;
	int history;
	int nmixtures;
	double varThreshold;
	double backgroundRatio;
	double noiseSigma;
public:
	void modifyMask(cv::Mat& mask);
	void saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count);
};

