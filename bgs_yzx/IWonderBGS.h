#pragma once
#include "IBGS.h"
#include "IWonder.h"
class CIWonderBGS :
	public IBGS
{
public:
	CIWonderBGS();
	~CIWonderBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	//yzxVibe* vibebgs;
	CIWonder* bgs;
	bool firstTime;
	bool showOutput;
};

