#include "stdafx.h"
#include "VibeTest.h"


CVibeTest::CVibeTest() :firstTime(true), showOutput(false)
{
}


CVibeTest::~CVibeTest()
{
}

void CVibeTest::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{

		firstTime = false;
	}
	vibebgs(img_input, img_output);
	vibebgs.saveBackgroundModels(img_bgmodel);
}

void CVibeTest::saveConfig()
{

}

void CVibeTest::loadConfig()
{

}
