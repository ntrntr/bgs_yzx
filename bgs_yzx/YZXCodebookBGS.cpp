#include "stdafx.h"
#include "YZXCodebookBGS.h"


CYZXCodebookBGS::CYZXCodebookBGS() :firstTime(true), learnLength(299), nframes(1)
{
	std::cout << "CYZXCodebookBGS()" << std::endl;
}


CYZXCodebookBGS::~CYZXCodebookBGS()
{
	//这个代码内存泄露
	if (cb)
	{
		delete[] cb;
	}

	std::cout << "~CYZXCodebookBGS()" << std::endl;
}

void CYZXCodebookBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
	cvtColor(img_input, yuvImage, CV_BGR2YCrCb);
	if (nframes <= learnLength)
	{
		pColor = (uchar*)yuvImage.data;
		pRGB = (uchar*)img_input.data;
		for (int c = 0; c < imageLen; ++c)
		{
			gbs.cvupdateCodeBook(pColor, pRGB, cb[c], cbBounds, nChannels);
			pColor += 3;
			pRGB += 3;
		}
		if (nframes == learnLength)
		{
			for (int c = 0; c < imageLen; ++c)
			{
				gbs.cvclearStaleEntries(cb[c]);
			}
		}
	}
	else
	{
		uchar maskPixelCodeBook;
		pColor = (uchar*)yuvImage.data;
		pRGB = (uchar*)img_input.data;
		uchar* pMask = (uchar*)maskCodeBook.data;
		for (int c = 0; c < imageLen; ++c)
		{
			maskPixelCodeBook = gbs.cvbackgroundDiff(pColor, pRGB, cb[c], nChannels, minMod, maxMod);
			*pMask++ = maskPixelCodeBook;
			pColor += 3;
			pRGB += 3;
		}
		if (nframes % learnLength == 0)
		{
			//for (int c = 0; c < imageLen; ++c)
			//{
			//	gbs.cvclearStaleEntries(cb[c]);
			//}
		}
	}
	//gbs.cvconnectedComponents3(maskCodeBook);
	//gbs.cvconnectedComponents3(maskCodeBook);
	maskCodeBook.copyTo(img_output);
	++nframes;
}

void CYZXCodebookBGS::initMemberVariable(const cv::Mat &img_input)
{
	yuvImage.create(img_input.size(), CV_8UC3);
	maskCodeBook.create(img_input.size(), CV_8UC1);
	maskCodeBook = cv::Scalar_<uchar>(255);
	imageLen = img_input.rows * img_input.cols;
	cb = new codeBook[imageLen];
	gbs.initialize(img_input);
	for (int i = 0; i < imageLen; ++i)
	{
		cb[i].numEntries = 0;
	}
	for (int i = 0; i < nChannels; ++i)
	{
		cbBounds[i] = 10;
		minMod[i] = 20;
		maxMod[i] = 20;
	}
}

void CYZXCodebookBGS::saveConfig()
{

}

void CYZXCodebookBGS::loadConfig()
{

}
