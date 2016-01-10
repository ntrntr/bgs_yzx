#pragma once
#include "IBGS.h"
#include "AMBER.h"
class CAMBERBGS :
	public IBGS
{
public:
	CAMBERBGS();
	~CAMBERBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	CAMBER *bgs;
	bool firstTime;
	bool showOutput;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
};

