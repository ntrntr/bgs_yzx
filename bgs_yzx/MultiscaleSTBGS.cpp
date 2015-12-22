#include "stdafx.h"
#include "MultiscaleSTBGS.h"


CMultiscaleSTBGS::CMultiscaleSTBGS() :firstTime(true), showOutput(false)
{
}


CMultiscaleSTBGS::~CMultiscaleSTBGS()
{
}

void CMultiscaleSTBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		init();
		firstTime = false;
	}
	bgs(img_input, img_output);
	bgs.getBackgroundModel(img_bgmodel);

}

void CMultiscaleSTBGS::init()
{

}

void CMultiscaleSTBGS::saveConfig()
{

}

void CMultiscaleSTBGS::loadConfig()
{

}
