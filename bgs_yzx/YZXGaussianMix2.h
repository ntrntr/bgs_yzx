#pragma once
#include "IBGS.h"
#include "MyGaussmix2.h"
class CYZXGaussianMix2 :
	public IBGS
{
public:
	CYZXGaussianMix2();
	~CYZXGaussianMix2();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void initMemberVariable(const cv::Mat &img_input);
private:
	void saveConfig();
	void loadConfig();
private:
	bool firstTime;
	int nframes;
	cv::Mat foreground;
	MyGaussmix2 gbs;
	double learningrate;
};

