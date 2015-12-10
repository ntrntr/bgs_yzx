#pragma once
#include "IBGS.h"
#include "Yzx.h"
class CMyBGS :
	public IBGS
{
public:
	CMyBGS();
	~CMyBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void init();
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	CYzx* bgs;
	bool firstTime;
	bool showOutput;
};

