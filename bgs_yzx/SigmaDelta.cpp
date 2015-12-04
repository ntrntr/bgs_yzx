#include "stdafx.h"
#include "SigmaDelta.h"


CSigmaDelta::CSigmaDelta() :firsttime(true), N(2)
{
}


CSigmaDelta::~CSigmaDelta()
{
}

void CSigmaDelta::operator()(const cv::Mat& image, cv::Mat& fgmask)
{
	Mat pFrame;
	image.copyTo(pFrame);
	//cvtColor(pFrame, pFrame, CV_BGR2GRAY);
	//GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
	pFrame.convertTo(pFrame, CV_32S);
	if (firsttime)
	{
		init(pFrame);
		firsttime = false;
	}
	for (int i = 0; i < rows; ++i)
	{
		int* pmean = m_Mean.ptr<int>(i);
		int* pdiff = m_Difference.ptr<int>(i);
		int* pimage = pFrame.ptr<int>(i);
		int* pvariance = m_Variance.ptr<int>(i);
		uchar* pforeimage = m_foreground.ptr<uchar>(i);
		for (int j = 0; j < cols; ++j)
		{
			int sumv = 0, sumdif = 0;
			for (int c = 0; c < channals; ++c)
			{
				*(pmean + j * channals + c) = *(pmean + j * channals + c) + getsgn(*(pimage + j * channals + c), *(pmean + j * channals + c));
				*(pdiff + j * channals + c) = std::abs((*(pmean + j * channals + c)) - (*(pimage + j * channals + c)));
				if (*(pdiff + j * channals + c) != 0)
				{
					*(pvariance + j * channals + c) = *(pvariance + j * channals + c) + getsgn((*(pdiff + j * channals + c)) * 2, *(pvariance + j * channals + c));
				}
				
				sumv += *(pvariance + j * channals + c);
				sumdif += *(pdiff + j * channals + c);
			}
			(*(pforeimage + j)) = sumdif < sumv ? 0 : 255;
		}
	}
	m_foreground.convertTo(fgmask, CV_8U);
	//m_foreground.copyTo(fgmask);
}

void CSigmaDelta::init(cv::Mat& pFrame)
{
	pFrame.copyTo(m_Mean);
	m_Mean.convertTo(m_Mean, CV_32SC3);
	m_Difference.create(pFrame.size(), CV_32SC3);
	m_Difference = Scalar::all(0);
	m_Variance.create(pFrame.size(), CV_32SC3);
	m_Variance = Scalar::all(0);
	m_foreground.create(pFrame.size(), CV_8UC1);
	m_foreground = Scalar::all(0);
	rows = pFrame.rows;
	cols = pFrame.cols;
	channals = pFrame.channels();
}

void CSigmaDelta::getBackgroundModel(cv::Mat& image)
{
	m_Mean.convertTo(image, CV_8U);
}

int CSigmaDelta::getsgn(int a, int b)
{
	if (a == b)
		return 0;
	else if (a < b)
	{
		return -1;
	}
	else
		return 1;
}
