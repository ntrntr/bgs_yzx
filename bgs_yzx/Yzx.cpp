#include "stdafx.h"
#include "Yzx.h"


CYzx::CYzx() :firstTime(true), N(30), m_min(2), meanLearningN(100), meanDCount(0), T_dec(0.05), T_inc(1.0), T_lower(2), T_upper(200), R_inc_dec(0.05), R_lower(18), R_scale(5.0)
{
	BGModel.backgroundModel.resize(N);
	BGModel.frequency.resize(N);
}


CYzx::~CYzx()
{
}

void CYzx::operator()(const cv::Mat& image, cv::Mat& fgmask)
{
	if (image.channels() == 3)
	{
		cvtColor(image, pFrame, CV_BGR2GRAY);
	}
	else
	{
		image.copyTo(pFrame);
	}
	GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
	if (firstTime)
	{
		init();
		firstTime = false;
	}
	float dist;
	for (int i = 0; i < rows; ++i)
	{
		uchar* p = pFrame.ptr<uchar>(i);
		float* pR = BGModel.R.ptr<float>(i);
		float* pmeanD = BGModel.meanD.ptr<float>(i);
		uchar* pMin = BGModel.minBrightness.ptr<uchar>(i);
		uchar* pMax = BGModel.maxBrightness.ptr<uchar>(i);
		uchar* pForeground = foreground.ptr<uchar>(i);
		float* pT = BGModel.T.ptr<float>(i);
		pBM.clear();
		pFrequency.clear();
		for (int ti = 0; ti < N; ++ti)
		{
			pBM.push_back(BGModel.backgroundModel[ti].ptr<float>(i));
			pFrequency.push_back(BGModel.frequency[ti].ptr<int>(i));
		}
		for (int j = 0; j < cols; ++j)
		{
			int count = 0;
			dist = 0;
			int index = 0;
			float minDist = 1000.0;
			while (index < N && count < m_min)
			{
				dist = std::abs(static_cast<float>(*(p + j)) - *(pBM[index] + j));
				if (dist < *(pR + j))
				{
					count++;
					++(*(pFrequency[index] + j));
				}
				minDist = std::min(minDist, dist);
				++index;
			}
			if (count >= m_min)
			{
				*(pForeground + j) = 0;
				//update mean
				if (meanDCount == 0)
				{
					*(pmeanD + j) = minDist;
					++meanDCount;
				}
				else if (meanDCount < meanLearningN)
				{
					*(pmeanD + j) = (*(pmeanD + j) * meanDCount + minDist) / (meanDCount + 1);
					++meanDCount;
				}
				else
				{
					*(pmeanD + j) = (*(pmeanD + j) * meanLearningN + minDist) / (meanLearningN + 1);
				}
				//1 / T to update the model
				int rdm = rng.uniform((int)0, (int)(*(pT + j)));
				if (rdm == 0)
				{
					updateBackgroundModel(i, j);
				}
				rdm = rng.uniform((int)0, (int)(*(pT + j)));
				if (rdm == 0)
				{
					updateNeighborModel(i, j);
				}
				updateT(0, (pT + j), (pmeanD + j));
			}
			else
			{
				*(pForeground + j) = 255;
				updateT(1, (pT + j), (pmeanD + j));
			}
			updateR(pR + j, pmeanD + j);
			
		}
	}
	foreground.copyTo(fgmask);
}

void CYzx::getBackgroundModel(cv::Mat& image)
{
	image.create(pFrame.size(), CV_32FC1);
	image = cv::Scalar::all(0);
	for (int i = 0; i < N; ++i)
	{
		image += BGModel.backgroundModel[i];
	}
	image.convertTo(image, CV_8U, 1.0 / N, 0.0);
}

void CYzx::initBackground()
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < rows; ++j)
		{
			float* pBM = BGModel.backgroundModel[i].ptr<float>(j);

			for (int k = 0; k < cols; ++k)
			{
				*(pBM + k) = static_cast<float>(getNeighbor(j, k));
			}
		}
		BGModel.frequency[i] = Scalar::all(1);
	}
}

void CYzx::init()
{
	BGModel.maxBrightness.create(pFrame.size(), CV_8UC1);
	pFrame.copyTo(BGModel.maxBrightness);
	BGModel.minBrightness.create(pFrame.size(), CV_8UC1);
	pFrame.copyTo(BGModel.minBrightness);
	for (int i = 0; i < N; ++i)
	{
		BGModel.backgroundModel[i].create(pFrame.size(), CV_32FC1);
		BGModel.frequency[i].create(pFrame.size(), CV_32SC1);
	}
	rows = pFrame.rows;
	cols = pFrame.cols;
	BGModel.meanD.create(pFrame.size(), CV_32FC1);
	BGModel.meanD = Scalar::all(0);
	BGModel.R.create(pFrame.size(), CV_32FC1);
	BGModel.R = cv::Scalar::all(R_lower);
	foreground.create(pFrame.size(), CV_8UC1);
	foreground = Scalar::all(0); 
	BGModel.T.create(pFrame.size(), CV_32FC1);
	BGModel.T = Scalar::all(16.0);
	initBackground();
}

void CYzx::modifyMask(cv::Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 5);
}

uchar CYzx::getNeighbor(int x, int y)
{
	x = x + rng.uniform((int)-1, (int)2);
	if (x < 0)
	{
		x = 0;
	}
	else if (x >= rows)
	{
		x = rows - 1;
	}
	y = y + rng.uniform((int)-1, (int)2);
	if (y < 0)
	{
		y = 0;
	}
	else if (y >= cols)
	{
		y = cols - 1;
	}
	return pFrame.at<uchar>(x, y);
}

void CYzx::updateBackgroundModel(int x, int y)
{
	int minFre = INT_MAX;
	int minIndex = -1;
	for (int i = 0; i < N; ++i)
	{
		if (BGModel.frequency[i].at<int>(x,y) < minFre)
		{
			minFre = BGModel.frequency[i].at<int>(x, y);
			minIndex = i;
		}
	}
	BGModel.backgroundModel[minIndex].at<float>(x, y) = pFrame.at<uchar>(x, y);
	BGModel.frequency[minIndex].at<int>(x, y) = 1;
}

void CYzx::updateNeighborModel(int x, int y)
{
	int rdm = rng.uniform(0, N);
	//himself
	//BGModel.backgroundModel[rdm].at<float>(x, y) = getNeighbor(x, y);
	//current data
	BGModel.backgroundModel[rdm].at<float>(getNeighborX(x, -2, 3), getNeighborY(y, -2, 3)) = pFrame.at<uchar>(x, y);
	BGModel.frequency[rdm].at<int>(x, y) = 1;
}

void CYzx::updateT(int flag, float* pt, float* pmean)
{
	if (flag == 0)
	{
		*pt = std::max(std::min(T_upper, *pt - T_dec / *pmean), T_lower);
	}
	else if (flag == 1)
	{
		*pt = std::max(std::min(T_upper, *pt + T_inc / *pmean), T_lower);
	}
}

void CYzx::updateR(float* pr, float* pmean)
{
	if ( *pr > *pmean * R_scale)
	{
		*pr = std::max(*pr * (1 - R_inc_dec), R_lower);
	}
	else
	{
		*pr = std::min(*pr * (1 + R_inc_dec), R_lower);
	}
}


int CYzx::getNeighborX(int x, int begin /*= -1*/, int end /*= 2*/)
{
	x = x + rng.uniform(begin, end);
	if (x < 0)
	{
		x = 0;
	}
	else if (x >= rows)
	{
		x = rows - 1;
	}
	return x;
}

int CYzx::getNeighborY(int y, int begin /*= -1*/, int end /*= 2*/)
{
	y = y + rng.uniform(begin, end);
	if (y < 0)
	{
		y = 0;
	}
	else if (y >= cols)
	{
		y = cols - 1;
	}
	return y;
}
