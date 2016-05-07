#pragma once
#include "IBGS.h"
#include "LSD.h"
class CLSDBGS :
	public IBGS
{
public:
	CLSDBGS();
	~CLSDBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	CLSD* bgs;
	bool firstTime;
	bool showOutput;
};

