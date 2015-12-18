#include "stdafx.h"
#include "CodebookOpencvBGS.h"


CCodebookOpencvBGS::CCodebookOpencvBGS() :firstTime(true), learnLength(100), nframes(1), nChannels(CHANNELS)
{
}


CCodebookOpencvBGS::~CCodebookOpencvBGS()
{
	if (cB)
	{
		delete[] cB;
	}
}

void CCodebookOpencvBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
		for (int c = 0; c < imageLen; c++)
		{
			cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);
			pColor += 3;

		}
		if (nframes == learnLength)
		{
			for (int c = 0; c < imageLen; ++c)
			{
				//cvclearStaleEntries(cB[c]);
			}
		}
	}
	else
	{
		uchar maskPixelCodeBook;
		pColor = (uchar*)yuvImage.data;
		uchar* pMask = (uchar*)ImaskCodeBook.data;
		for (int c = 0; c < imageLen; ++c)
		{
			maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, minMod, maxMod);
			*pMask++ = maskPixelCodeBook;
			pColor += 3;
		}
	}
	ImaskCodeBook.copyTo(img_output);
	++nframes;
}

void CCodebookOpencvBGS::initMemberVariable(const cv::Mat &img_input)
{
	img_input.copyTo(rawImage);
	yuvImage.create(img_input.size(), CV_8UC3);
	ImaskCodeBook.create(img_input.size(), CV_8UC1);
	ImaskCodeBook = Scalar::all(255);
	imageLen = rawImage.rows * rawImage.cols;
	cB = new codeBook[imageLen];
	for (int i = 0; i < imageLen; ++i)
	{
		cB[i].numEntries = 0;
	}
	for (int i = 0; i < nChannels; ++i)
	{
		cbBounds[i] = 10;
		minMod[i] = 20;
		maxMod[i] = 20;
	}
}

void CCodebookOpencvBGS::saveConfig()
{

}

void CCodebookOpencvBGS::loadConfig()
{

}
