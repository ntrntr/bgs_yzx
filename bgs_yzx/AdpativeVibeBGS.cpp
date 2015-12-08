#include "stdafx.h"
#include "AdpativeVibeBGS.h"


CAdpativeVibeBGS::CAdpativeVibeBGS() :firstTime(true), showOutput(true), vibebgs(NULL), alpha(0.5)
{

}


CAdpativeVibeBGS::~CAdpativeVibeBGS()
{
	if (vibebgs)
	{
		delete vibebgs;
	}

}

void CAdpativeVibeBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime)
	{
		saveConfig();
		vibebgs = new CAdpativeyzxVibe();
	}
	(*vibebgs)(img_input, img_output);
	vibebgs->saveBackgroundModels(img_bgmodel);
	firstTime = false;
}

void CAdpativeVibeBGS::saveConfig()
{
	//CvFileStorage* fs = cvOpenFileStorage("./config/adpative_vibe.xml", 0, CV_STORAGE_WRITE);
	//cvWriteReal(fs, "alpha", alpha);
	//cvReleaseFileStorage(&fs);
}

void CAdpativeVibeBGS::loadConfig()
{
	//CvFileStorage* fs = cvOpenFileStorage("./config/adpative_vibe.xml", 0, CV_STORAGE_READ);
	//alpha = cvReadRealByName(fs, 0, "alpha", 0.5);
	//cvReleaseFileStorage(&fs);
	alpha = 0.5;
}

