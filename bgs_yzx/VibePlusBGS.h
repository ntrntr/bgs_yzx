#pragma once
#include "IBGS.h"
#include "VibePlus.h"
class CVibePlusBGS :
	public IBGS
{
public:
	CVibePlusBGS();
	~CVibePlusBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	CVibePlus* vibebgs;
	bool firstTime;
	bool showOutput;
};

