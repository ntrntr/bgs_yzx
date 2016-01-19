#pragma once
#include "IBGS.h"
#include "TBackgroundVuMeter.h"
class CVuMeterBGS :
	public IBGS
{
public:
	CVuMeterBGS();
	~CVuMeterBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
public:
	bool firstTime;
	bool showOutput;
	double alpha;
	double threshold;
	int binSize;
	bool enableFilter;
	TBackgroundVuMeter bgs;
	Mat frame;
	Mat gray;
	Mat background;
	Mat mask;
};

