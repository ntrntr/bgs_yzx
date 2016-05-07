#include "stdafx.h"
#include "LobsterBGS.h"


CLobsterBGS::CLobsterBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CLobsterBGS::~CLobsterBGS()
{
}

void CLobsterBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		saveConfig();
		bgs = new BackgroundSubtractorLOBSTER();
		bgs->initialize(img_input, Mat(img_input.size(), CV_8UC1, Scalar_<uchar>(255)));
		firstTime = false;
	}
	bgs->operator()(img_input, img_output);
	bgs->getBackgroundImage(img_bgmodel);
}

void CLobsterBGS::saveConfig()
{

}

void CLobsterBGS::loadConfig()
{

}
