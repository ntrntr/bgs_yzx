#include "stdafx.h"
#include "SigmaDeltaBGS.h"


CSigmaDeltaBGS::CSigmaDeltaBGS() :firstTime(true), showOutput(true), sdbgs(NULL)
{
	std::cout << "CSigmaDeltaBGS()" << std::endl;
}


CSigmaDeltaBGS::~CSigmaDeltaBGS()
{
	if (sdbgs)
	{
		delete sdbgs;
	}
	std::cout << "~CSigmaDeltaBGS()" << std::endl;
}

void CSigmaDeltaBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		saveConfig();
		sdbgs = new CSigmaDelta();
		firstTime = false;
	}
	(*sdbgs)(img_input, img_output);
	sdbgs->getBackgroundModel(img_bgmodel);
}

void CSigmaDeltaBGS::saveConfig()
{

}

void CSigmaDeltaBGS::loadConfig()
{

}
