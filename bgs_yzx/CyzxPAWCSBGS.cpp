#include "stdafx.h"
#include "CyzxPAWCSBGS.h"


CyzxPAWCSBGS::CyzxPAWCSBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CyzxPAWCSBGS::~CyzxPAWCSBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void CyzxPAWCSBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		saveConfig();
		bgs = new CYzxPAWCS();
		bgs->init(img_input, Mat(img_input.size(), CV_8UC1, Scalar_<uchar>(255)));
		firstTime = false;
	}
	bgs->operator()(img_input, img_output, double(100.0));
	bgs->getBackgroundImage(img_bgmodel);
}

void CyzxPAWCSBGS::saveConfig()
{

}

void CyzxPAWCSBGS::loadConfig()
{

}
