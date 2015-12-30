#pragma once
#include "IBGS.h"
#include "YzxLBSP.h"
class CYzxLBSPBGS :
	public IBGS
{
public:
	CYzxLBSPBGS();
	~CYzxLBSPBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	//BackgroundSubtractorPAWCS bgsPAWCS;
	CYzxLBSP* bgs;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

