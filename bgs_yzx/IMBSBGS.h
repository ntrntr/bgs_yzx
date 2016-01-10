#pragma once
#include "IBGS.h"
#include "IMBS.h"
class CIMBSBGS :
	public IBGS
{
public:
	CIMBSBGS();
	~CIMBSBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	CIMBS *bgs;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

