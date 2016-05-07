#include "stdafx.h"
#include "PAWCSBgs.h"


CPAWCSBgs::CPAWCSBgs() : firstTime(true), showOutput(false)
{
}


CPAWCSBgs::~CPAWCSBgs()
{

}

void CPAWCSBgs::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		saveConfig();
		oCurrSegmMask.create(img_input.size(), CV_8UC1);
		oCurrReconstrBGImg.create(img_input.size(), img_input.type());
		bgsPAWCS.initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
		firstTime = false;
	}
	bgsPAWCS(img_input, img_output, double(100.0));
	bgsPAWCS.getBackgroundImage(img_bgmodel);
}

void CPAWCSBgs::saveConfig()
{

}

void CPAWCSBgs::loadConfig()
{

}
