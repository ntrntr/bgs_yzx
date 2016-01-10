#include "stdafx.h"
#include "IMBS.h"


CIMBS::CIMBS() :pIMBS(NULL)
{
}


CIMBS::~CIMBS()
{
	if (pIMBS)
	{
		delete pIMBS;
	}
}
void CIMBS::init()
{
	pIMBS = new BackgroundSubtractorIMBS(0);
}
void CIMBS::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate /*= 0*/)
{
	pIMBS->apply(image, fgmask);
}

void CIMBS::getBackgroundImage(cv::Mat& image)
{
	pIMBS->getBackgroundImage(image);
}


