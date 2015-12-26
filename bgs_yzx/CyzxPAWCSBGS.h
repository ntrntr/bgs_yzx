#pragma once
#include "IBGS.h"
#include "YzxPAWCS.h"
class CyzxPAWCSBGS :
	public IBGS
{
public:
	CyzxPAWCSBGS();
	~CyzxPAWCSBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	CYzxPAWCS *bgs;
	//BackgroundSubtractorPAWCS bgsPAWCS;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

