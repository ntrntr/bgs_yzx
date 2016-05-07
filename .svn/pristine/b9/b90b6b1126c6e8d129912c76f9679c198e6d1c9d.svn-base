#include "stdafx.h"
#include "IMBSBGS.h"


CIMBSBGS::CIMBSBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
}


CIMBSBGS::~CIMBSBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void CIMBSBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		saveConfig();
		bgs = new CIMBS();
		bgs->init();
		firstTime = false;
	}
	bgs->operator()(img_input, img_output);
	bgs->getBackgroundImage(img_bgmodel);
}

void CIMBSBGS::saveConfig()
{

}

void CIMBSBGS::loadConfig()
{

}
