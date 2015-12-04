#pragma once
#include "IBGS.h"
#include "AdpativeyzxVibe.h"
class CAdpativeVibeBGS :
	public IBGS
{
public:
	CAdpativeVibeBGS();
	~CAdpativeVibeBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	CAdpativeyzxVibe* vibebgs;
	bool firstTime;
	bool showOutput;
	double alpha;
};

