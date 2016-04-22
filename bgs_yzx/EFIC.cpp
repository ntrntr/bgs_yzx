#include "stdafx.h"
#include "EFIC.h"


CEFIC::CEFIC()
{
}


CEFIC::~CEFIC()
{
}

void CEFIC::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate /*= 0*/)
{
	if (image.channels() == 3)
	{
		cvtColor(image, pFrame, CV_BGR2GRAY);
	}
	GaussianBlur(pFrame, pFrame, cv::Size(3, 3), 0);
	fgmask.create(pFrame.size(), CV_8UC1);
	fgmask = cv::Scalar::all(0);
}

void CEFIC::saveBackgroundModels(cv::Mat& image)
{
	pFrame.copyTo(image);
}
