#include "stdafx.h"
#include "LSDBGS.h"


CLSDBGS::CLSDBGS():firstTime(true), showOutput(false), bgs(NULL)
{
}


CLSDBGS::~CLSDBGS()
{
	if (bgs)
	{
		delete bgs;
	}
}

void CLSDBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
		bgs = new CLSD();
	}
	(*bgs)(img_input, img_output);
	bgs->saveBackgroundModels(img_bgmodel);
}

void CLSDBGS::saveConfig()
{

}

void CLSDBGS::loadConfig()
{

}
