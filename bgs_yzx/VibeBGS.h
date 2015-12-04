#pragma once
#include "IBGS.h"
#include "yzxVibe.h"
class VibeBGS :
	public IBGS
{
public:
	VibeBGS();
	~VibeBGS();

public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	yzxVibe* vibebgs;
	bool firstTime;
	bool showOutput;
};

