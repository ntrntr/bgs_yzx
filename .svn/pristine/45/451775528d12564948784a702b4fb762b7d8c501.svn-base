#include "stdafx.h"
#include "PBASBGS.h"


CPBASBGS::CPBASBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
	cout << "CPBASBGS()" << endl;
}


CPBASBGS::~CPBASBGS()
{
	if (bgs)
	{
		delete bgs;
	}
	cout << "~CPBASBGS()" << endl;
}

void CPBASBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		saveConfig();
		bgs = new CPBAS();
		firstTime = false;
	}
	(*bgs)(img_input, img_output, img_bgmodel);
}

void CPBASBGS::saveConfig()
{

}

void CPBASBGS::loadConfig()
{

}
