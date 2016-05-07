#include "stdafx.h"
#include "AMBERBGS.h"


CAMBERBGS::CAMBERBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CAMBERBGS::~CAMBERBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void CAMBERBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		saveConfig();
		bgs = new CAMBER();
		firstTime = false;
	}
	bgs->operator()(img_input, img_output);
	bgs->getBackgroundImage(img_bgmodel);
}

void CAMBERBGS::saveConfig()
{

}

void CAMBERBGS::loadConfig()
{

}
