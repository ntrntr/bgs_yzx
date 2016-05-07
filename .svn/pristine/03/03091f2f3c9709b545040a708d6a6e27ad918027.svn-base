#pragma once
#include "IBGS.h"
#include "BackgroundSubtractorLOBSTER.h"
class CLobsterBGS :
	public IBGS
{
public:
	CLobsterBGS();
	~CLobsterBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	BackgroundSubtractorLOBSTER *bgs;
	//BackgroundSubtractorPAWCS bgsPAWCS;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

