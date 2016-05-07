#include "stdafx.h"
#include "YZXAdpativeGaussianMix.h"


CYZXAdpativeGaussianMix::CYZXAdpativeGaussianMix() :firstTime(true), nframes(0), learningrate(0.002)
{
	std::cout << "CYZXGaussianMix()" << std::endl;
}


CYZXAdpativeGaussianMix::~CYZXAdpativeGaussianMix()
{
	std::cout << "~CYZXGaussianMix()" << std::endl;
}

void CYZXAdpativeGaussianMix::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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

void CYZXAdpativeGaussianMix::initMemberVariable(const cv::Mat &img_input)
{

}

void CYZXAdpativeGaussianMix::saveConfig()
{

}

void CYZXAdpativeGaussianMix::loadConfig()
{

}