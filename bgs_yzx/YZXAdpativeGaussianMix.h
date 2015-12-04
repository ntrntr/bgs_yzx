#pragma once
#include "IBGS.h"
#include "MyAdpativeMOG.h"
class CYZXAdpativeGaussianMix :
	public IBGS
{
public:
	CYZXAdpativeGaussianMix();
	~CYZXAdpativeGaussianMix();
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
	CMyAdpativeMOG gbs;
	double learningrate;
};

