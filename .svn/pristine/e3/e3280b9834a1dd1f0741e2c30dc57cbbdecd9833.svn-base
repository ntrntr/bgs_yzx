#include "stdafx.h"
#include "VibePlusBGS.h"




CVibePlusBGS::CVibePlusBGS() :firstTime(true), showOutput(true), vibebgs(NULL)
{

}


CVibePlusBGS::~CVibePlusBGS()
{
	if (vibebgs)
	{
		delete vibebgs;
	}

}

void CVibePlusBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime)
	{
		saveConfig();
		vibebgs = new CVibePlus();
	}
	(*vibebgs)(img_input, img_output);
	vibebgs->saveBackgroundModels(img_bgmodel);
	firstTime = false;
}

void CVibePlusBGS::saveConfig()
{

}

void CVibePlusBGS::loadConfig()
{

}