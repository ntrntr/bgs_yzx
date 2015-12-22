#pragma once
#include "IBGS.h"
#include "BackgroundSubtractorPAWCS.h"
class CPAWCSBgs :
	public IBGS
{
public:
	CPAWCSBgs();
	~CPAWCSBgs();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	BackgroundSubtractorPAWCS bgsPAWCS;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

