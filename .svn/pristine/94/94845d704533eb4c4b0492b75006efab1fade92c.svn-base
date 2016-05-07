#pragma once
#include "IBGS.h"
#include "SigmaDelta.h"
class CSigmaDeltaBGS :
	public IBGS
{
public:
	CSigmaDeltaBGS();
	~CSigmaDeltaBGS();

public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
private:
	//BackgroundSubtractorLOBSTER* pLOBSTER;
	//yzxVibe* vibebgs;
	CSigmaDelta* sdbgs;
	bool firstTime;
	bool showOutput;
};

