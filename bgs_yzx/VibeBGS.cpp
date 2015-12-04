#include "stdafx.h"
#include "VibeBGS.h"


VibeBGS::VibeBGS() :firstTime(true), showOutput(true), vibebgs(NULL)
{

}


VibeBGS::~VibeBGS()
{
	if (vibebgs)
	{
		delete vibebgs;
	}
	
}

void VibeBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime)
	{
		saveConfig();
		vibebgs = new yzxVibe();
	}
	(*vibebgs)(img_input, img_output);
	vibebgs->saveBackgroundModels(img_bgmodel);
	firstTime = false;
}

void VibeBGS::saveConfig()
{

}

void VibeBGS::loadConfig()
{

}
