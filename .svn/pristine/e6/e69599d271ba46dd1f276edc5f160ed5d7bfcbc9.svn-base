#include "stdafx.h"
#include "EFICBGS.h"


EFICBGS::EFICBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


EFICBGS::~EFICBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void EFICBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		firstTime = false;
		saveConfig();
		bgs = new CEFIC();
	}
	(*bgs)(img_input, img_output);
	bgs->saveBackgroundModels(img_bgmodel);
}

void EFICBGS::saveConfig()
{

}

void EFICBGS::loadConfig()
{

}
