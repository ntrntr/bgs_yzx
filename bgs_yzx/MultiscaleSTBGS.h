#pragma once
#include "IBGS.h"
#include "MultiscaleSTBG.h"
class CMultiscaleSTBGS :
	public IBGS
{
public:
	CMultiscaleSTBGS();
	~CMultiscaleSTBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void init();
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	bool firstTime;
	bool showOutput;
	CMultiscaleSTBG bgs;
};

