#pragma once
#include "IBGS.h"
#include "EFIC.h"
class EFICBGS :
	public IBGS
{
public:
	EFICBGS();
	~EFICBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	CEFIC* bgs;
	bool firstTime;
	bool showOutput;
};

