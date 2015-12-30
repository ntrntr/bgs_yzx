#include "stdafx.h"
#include "YzxLBSPBGS.h"


CYzxLBSPBGS::CYzxLBSPBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CYzxLBSPBGS::~CYzxLBSPBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void CYzxLBSPBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		loadConfig();
		saveConfig();
		bgs = new CYzxLBSP();
		bgs->initialize(img_input, Mat(img_input.size(), CV_8UC1, Scalar_<uchar>(255)));
		firstTime = false;
	}
	bgs->operator()(img_input, img_output);
	bgs->getBackgroundImage(img_bgmodel);
	

}

void CYzxLBSPBGS::saveConfig()
{

}

void CYzxLBSPBGS::loadConfig()
{

}
