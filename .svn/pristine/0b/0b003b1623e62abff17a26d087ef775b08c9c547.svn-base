#include "stdafx.h"
#include "IWonderBGS.h"


CIWonderBGS::CIWonderBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CIWonderBGS::~CIWonderBGS()
{
}

void CIWonderBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		loadConfig();
		saveConfig();
		bgs = new CIWonder();
		firstTime = false;
	}
	(*bgs)(img_input, img_output);
	bgs->getBackgroundImage(img_bgmodel);
}

void CIWonderBGS::saveConfig()
{

}

void CIWonderBGS::loadConfig()
{

}
