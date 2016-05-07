#include "stdafx.h"
#include "YZXGaussianMix2.h"


CYZXGaussianMix2::CYZXGaussianMix2() :firstTime(true), nframes(0), learningrate(0.002)
{
	std::cout << "CYZXGaussianMix2()" << std::endl;
}


CYZXGaussianMix2::~CYZXGaussianMix2()
{
	std::cout << "~CYZXGaussianMix()2" << std::endl;
}

void CYZXGaussianMix2::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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

void CYZXGaussianMix2::initMemberVariable(const cv::Mat &img_input)
{

}

void CYZXGaussianMix2::saveConfig()
{

}

void CYZXGaussianMix2::loadConfig()
{

}
