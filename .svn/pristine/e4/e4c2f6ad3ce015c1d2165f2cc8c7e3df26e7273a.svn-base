#pragma once
#include "IBGS.h"
class CAdpativeBGSubtract :
	public IBGS
{
public:
	CAdpativeBGSubtract();
	~CAdpativeBGSubtract();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void updateBackgroundModel(const cv::Mat& img_diff, cv::Mat& img_input);
private:
	void saveConfig();
	void loadConfig();
private:
	bool firstTime;
	cv::Mat img_background;
	cv::Mat img_forground;
	double alpha;
	long limit;
	long counter;
	bool enableThreshold;
	int threshold;
	bool showForeground;
	bool showBackground;
};

