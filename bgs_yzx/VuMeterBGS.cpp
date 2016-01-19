#include "stdafx.h"
#include "VuMeterBGS.h"


CVuMeterBGS::CVuMeterBGS() :firstTime(true), showOutput(false), binSize(8), alpha(0.995), threshold(0.03), enableFilter(true)
{
}


CVuMeterBGS::~CVuMeterBGS()
{
}

void CVuMeterBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		loadConfig();
		saveConfig();
		return;
	}
	if (firstTime)
	{
		bgs.SetAlpha(alpha);
		bgs.SetBinSize(binSize);
		bgs.SetThreshold(threshold);
		gray.create(img_input.size(), CV_8UC1);
		cvtColor(img_input, gray, CV_BGR2GRAY);
		background.create(img_input.size(), CV_8UC1);
		gray.copyTo(background);
		mask.create(img_input.size(), CV_8UC1);
		mask = Scalar::all(0);
		firstTime = false;
	}
	else
	{
		cvtColor(img_input, gray, CV_BGR2GRAY);
		
	}
	//&yuvImage.operator IplImage()
	bgs.UpdateBackground(&gray.operator IplImage(), &background.operator IplImage(), &mask.operator IplImage());
	Mat img_foreground(mask);
	Mat img_bkg(background);
	if (enableFilter)
	{
		erode(img_foreground, img_foreground, Mat());
		medianBlur(img_foreground, img_foreground, 5);
	}

	img_foreground.copyTo(img_output);
	img_bkg.copyTo(img_bgmodel);
}

void CVuMeterBGS::saveConfig()
{

}

void CVuMeterBGS::loadConfig()
{

}
