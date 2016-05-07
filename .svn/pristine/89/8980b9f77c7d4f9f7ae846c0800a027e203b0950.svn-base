#pragma once
#include "IBGS.h"
#include "ImproveVibeTest.h"
class CVibeTest :
	public IBGS
{
public:
	CVibeTest();
	~CVibeTest();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	//yzxVibe* vibebgs;
	CImproveVibeTest vibebgs;
	bool firstTime;
	bool showOutput;
};

