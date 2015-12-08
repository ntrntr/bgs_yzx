#pragma once
#include "IBGS.h"
#include "PBAS.h"
class CPBASBGS :
	public IBGS
{
public:
	CPBASBGS();
	~CPBASBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	//yzxVibe* vibebgs;
	CPBAS* bgs;
	bool firstTime;
	bool showOutput;
};

