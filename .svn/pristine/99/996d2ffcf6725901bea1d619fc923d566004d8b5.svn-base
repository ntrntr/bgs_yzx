#include "stdafx.h"
#include "YZXGaussianMix.h"


CYZXGaussianMix::CYZXGaussianMix() :firstTime(true), nframes(0), learningrate(0.0033)
{
	std::cout << "CYZXGaussianMix()" << std::endl;
}


CYZXGaussianMix::~CYZXGaussianMix()
{
	std::cout << "~CYZXGaussianMix()" << std::endl;
}

void CYZXGaussianMix::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime)
	{
		saveConfig();
		initMemberVariable(img_input);
		firstTime = false;
	}
	gbs(img_input, img_output, learningrate);
	gbs.getBackgroundImage(img_bgmodel);
}

void CYZXGaussianMix::initMemberVariable(const cv::Mat &img_input)
{

}

void CYZXGaussianMix::saveConfig()
{

}

void CYZXGaussianMix::loadConfig()
{

}
