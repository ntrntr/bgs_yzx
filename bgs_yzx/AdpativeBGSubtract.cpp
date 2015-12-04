#include "stdafx.h"
#include "AdpativeBGSubtract.h"


CAdpativeBGSubtract::CAdpativeBGSubtract() :firstTime(true), alpha(0.5), limit(-1), counter(0), enableThreshold(true),
threshold(20), showForeground(true), showBackground(true)
{
	std::cout << "CAdpativeBGSubtract()" << std::endl;
}


CAdpativeBGSubtract::~CAdpativeBGSubtract()
{
	std::cout << "~CAdpativeBGSubtract()" << std::endl;
}

void CAdpativeBGSubtract::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		saveConfig();
		firstTime = false;
	}
	cv::Mat input;
	img_input.copyTo(input);
	//GaussianBlur(img_input, input, cv::Size(3, 3), 0);
	cv::cvtColor(input, input, CV_RGB2GRAY);
	input.convertTo(input, CV_32F);
	if (img_background.empty())
	{
		input.copyTo(img_background);
	}
	cv::absdiff(input, img_background, img_forground);
	//if (img_forground.channels() == 3)
	//{
	//	cv::cvtColor(img_forground, img_forground, CV_RGB2GRAY);
	//}
	cv::threshold(img_forground, img_forground, threshold, 255, cv::THRESH_BINARY);
	img_forground.convertTo(img_forground, CV_8U);
	img_forground.copyTo(img_output);
	updateBackgroundModel(img_forground, input);
	img_background.copyTo(img_bgmodel);
	img_bgmodel.convertTo(img_bgmodel, CV_8U);
}

void CAdpativeBGSubtract::updateBackgroundModel(const cv::Mat& img_diff, cv::Mat& img_input)
{
	float* data;
	const uchar* pmask;
	const float* pinput;
	int nchannels = img_background.channels();
	for (int y = 0; y < img_background.rows; ++y)
	{

		data = img_background.ptr<float>(y);
		pmask = img_diff.ptr<uchar>(y);
		pinput = img_input.ptr<float>(y);
		for (int x = 0; x < img_background.cols; ++x, data += nchannels, pinput += nchannels, pmask++)
		{
			if (*pmask > 0)
			{
				continue;
			}
			else
			{
				for (int c = 0; c < nchannels; ++c)
				{
					data[c] = ((1.0 - alpha) * data[c] + alpha * pinput[c]);
				}
			}
		}
	}
}

void CAdpativeBGSubtract::saveConfig()
{

}

void CAdpativeBGSubtract::loadConfig()
{

}
