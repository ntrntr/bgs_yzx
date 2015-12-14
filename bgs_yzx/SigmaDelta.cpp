#include "stdafx.h"
#include "SigmaDelta.h"
#include <fstream>
CSigmaDelta::CSigmaDelta() :firsttime(true), N(2), alpha(0.5)
{
	//Point pt(100, 100);
	kindOfPoint = 6;
	watchPoint.push_back(Point(680,364));
	watchPoint.push_back(Point(579,60));
	resultPoint.assign(watchPoint.size(), vector<vector<int>>(kindOfPoint));
}


CSigmaDelta::~CSigmaDelta()
{
	ofstream outfile;
	outfile.open("SigmaDeltaLog.txt");
	
	for (int i = 0; i < watchPoint.size(); ++i)
	{
		outfile << "point : x = " <<watchPoint[i].x <<", y = "<<watchPoint[i].y<<endl;

		for (auto j:resultPoint[i])
		{
			for (auto k:j)
			{
				outfile << k << " ";
			}
			outfile << endl;
		}
	}
	outfile.close();
}

void CSigmaDelta::operator()(const cv::Mat& image, cv::Mat& fgmask)
{
	Mat pFrame;
	image.copyTo(pFrame);
	cvtColor(pFrame, pFrame, CV_BGR2GRAY);
	GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
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
		float* pMA = m_MovingAverage.ptr<float>(i);
		float* pNMA = m_NonlinearMoving.ptr<float>(i);
		int* pDilation = m_Dilation.ptr<int>(i);
		int* pErosion = m_Erosion.ptr<int>(i);
		uchar* pforeimage = m_foreground.ptr<uchar>(i);
		for (int j = 0; j < cols; ++j)
		{
			int sumv = 0, sumdif = 0;
			for (int c = 0; c < channals; ++c)
			{
				//sigma delta mean
				*(pmean + j * channals + c) = *(pmean + j * channals + c) + getsgn(*(pimage + j * channals + c), *(pmean + j * channals + c));
				//moving average
				*(pMA + j * channals + c) = *(pMA + j * channals + c) * (1 - alpha) + alpha * *(pimage + j * channals + c);
				//nonlinear moving average
				double tmp = std::pow(alpha, std::abs(*(pNMA + j * channals + c) - *(pimage + j * channals + c)));
				*(pNMA + j * channals + c) = *(pNMA + j * channals + c) * (1 - tmp) + *(pimage + j * channals + c) * tmp;
				//dilation
				*(pDilation + j * channals + c) = std::max(*(pDilation + j * channals + c), *(pimage + j * channals + c));
				//erosion
				*(pErosion + j * channals + c) = std::min(*(pErosion + j * channals + c), *(pimage + j * channals + c));
				*(pdiff + j * channals + c) = std::abs((*(pmean + j * channals + c)) - (*(pimage + j * channals + c)));
				if (*(pdiff + j * channals + c) != 0)
				{
					*(pvariance + j * channals + c) = *(pvariance + j * channals + c) + getsgn((*(pdiff + j * channals + c)) * 2, *(pvariance + j * channals + c));
				}
				if (*(pvariance + j * channals + c) < 15)
				{
					*(pvariance + j * channals + c) = 15;
				}
				if (*(pvariance + j * channals + c) > 255)
				{
					*(pvariance + j * channals + c) = 255;
				}
				
				sumv += *(pvariance + j * channals + c);
				sumdif += *(pdiff + j * channals + c);
			}
			(*(pforeimage + j)) = sumdif < sumv ? 0 : 255;
		}
	}
	m_foreground.convertTo(fgmask, CV_8U);
	//m_foreground.copyTo(fgmask);
	saveCustomedPointData(pFrame);
}

void CSigmaDelta::init(cv::Mat& pFrame)
{
	pFrame.copyTo(m_Mean);
	m_Mean.convertTo(m_Mean, CV_32S);
	m_Mean.copyTo(m_Dilation);
	m_Mean.copyTo(m_Erosion);
	m_Mean.convertTo(m_MovingAverage,CV_32F);
	m_Mean.convertTo(m_NonlinearMoving, CV_32F);
	
	m_Difference.create(pFrame.size(), CV_32SC1);
	m_Difference = Scalar::all(0);
	m_Variance.create(pFrame.size(), CV_32SC1);
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

void CSigmaDelta::saveCustomedPointData(cv::Mat& pFrame)
{
	for (int i = 0; i < watchPoint.size(); ++i)
	{
		/*for (int j = 0; j < kindOfPoint; ++j)
		{
			resultPoint[i][j].push_back(0);
		}*/
		resultPoint[i][0].push_back(pFrame.at<int>(watchPoint[i]));
		resultPoint[i][1].push_back(m_Mean.at<int>(watchPoint[i]));
		resultPoint[i][2].push_back(static_cast<int>(m_MovingAverage.at<float>(watchPoint[i])));
		resultPoint[i][3].push_back(static_cast<int>(m_NonlinearMoving.at<float>(watchPoint[i])));
		resultPoint[i][4].push_back(m_Dilation.at<int>(watchPoint[i]));
		resultPoint[i][5].push_back(m_Erosion.at<int>(watchPoint[i]));
	}
}
