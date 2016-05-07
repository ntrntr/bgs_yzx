#include "stdafx.h"
#include "MyBGS.h"


CMyBGS::CMyBGS() :firstTime(true), showOutput(false), bgs(NULL)
{
	cout << "CMyBGS()" << endl;
}


CMyBGS::~CMyBGS()
{
	if (bgs)
	{
		delete bgs;
	}
	cout << "~CMyBGS()" << endl;
}

void CMyBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	if (firstTime)
	{
		init();
		firstTime = false;
	}
	(*bgs)(img_input, img_output);
	bgs->getBackgroundModel(img_bgmodel);
}

void CMyBGS::init()
{
	bgs = new CYzx();
}

void CMyBGS::saveConfig()
{

}

void CMyBGS::loadConfig()
{

}
