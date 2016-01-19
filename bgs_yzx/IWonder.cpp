#include "stdafx.h"
#include "IWonder.h"


CIWonder::CIWonder() :firstTime(true), patchsize(5), needRebuiltModel(false), RThreshold(20.0), m_min(2), ts(16), incR(1), decR(-0.1), blinkTs(6), beta(0.70)
{
}


CIWonder::~CIWonder()
{
}

void CIWonder::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate /*= 0*/)
{
	//cvtColor(image, pFrame, CV_BGR2GRAY);
	GaussianBlur(image, pFrame, Size(3, 3), 0);
	mask.create(pFrame.size(), CV_8UC1);
	pyrDown(pFrame, pFrameDownSample);
	maskDownSample.create(pFrameDownSample.size(), CV_8UC1);
	if (firstTime)
	{
		init();
		firstTime = false;
	}

	if (needRebuiltModel)
	{
		initBackgroundModels();
		needRebuiltModel = false;
	}
#pragma omp sections
	{
#pragma omp section
		{
			for (int i = 0; i < frameSize.height; ++i)
			{
				vector<double> a(3, 0.0);
				uchar* pImage = pFrame.ptr<uchar>(i);
				float* pR = R.ptr<float>(i);
				uchar* pMask = mask.ptr<uchar>(i);
				Vec3b tmp;
				uchar* pB = pBlink.ptr<uchar>(i);
				for (int j = 0; j < frameSize.width; ++j, ++pR, pImage += 3, ++pMask, ++pB)
				{
					int count = 0;
					double dist = 0;
					int index = 0;
					while (count < m_min && index < IWonder_MODEL_SIZE)
					{
						tmp = backgroundModels[index].at<Vec3b>(i, j);
						a[0] = (int)pImage[0] - (int)tmp[0];
						a[1] = (int)pImage[1] - (int)tmp[1];
						a[2] = (int)pImage[2] - (int)tmp[2];
						dist = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
						if (dist < *pR)
						{
							++count;
						}
						++index;
					}

					if (count >= m_min)
					{
						*pMask = 0;
						int updateRate = (*pB)>0 ? blinkTs : ts;
						int rdm = random(0, updateRate);
						if (rdm == 0)
						{
							rdm = random(0, IWonder_MODEL_SIZE);
							backgroundModels[rdm].at<Vec3b>(i, j) = pFrame.at<Vec3b>(i, j);
						}
						rdm = random(0, updateRate);
						if (rdm == 0)
						{
							int x = 0;
							int y = 0;
							while (x == 0 && y == 0)
							{
								x = random((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
								y = random((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
							}
							x += i;
							y += j;
							if (x < 0)
							{
								x = 0;
							}
							else if (x >= frameSize.height)
							{
								x = frameSize.height - 1;
							}
							if (y < 0)
							{
								y = 0;
							}
							else if (y >= frameSize.width)
							{
								y = frameSize.width - 1;
							}
							rdm = random(0, IWonder_MODEL_SIZE);
							backgroundModels[rdm].at<Vec3b>(x, y) = pFrame.at<Vec3b>(i, j);
						}
					}
					else
					{
						*pMask = 255;
					}
				}
			}
		}
#pragma omp section
		{
			//downsample
			for (int i = 0; i < frameSizeDownSample.height; ++i)
			{
				vector<double> a(3, 0.0);
				uchar* pImage = pFrameDownSample.ptr<uchar>(i);
				float* pR = RDownSample.ptr<float>(i);
				uchar* pMask = maskDownSample.ptr<uchar>(i);
				Vec3b tmp;
				uchar* pB = pBlinkDownSample.ptr<uchar>(i);
				for (int j = 0; j < frameSizeDownSample.width; ++j, ++pR, pImage += 3, ++pMask, ++pB)
				{
					int count = 0;
					double dist = 0;
					int index = 0;
					while (count < m_min && index < IWonder_MODEL_SIZE)
					{
						tmp = backgroundModelsDownSample[index].at<Vec3b>(i, j);
						a[0] = (int)pImage[0] - (int)tmp[0];
						a[1] = (int)pImage[1] - (int)tmp[1];
						a[2] = (int)pImage[2] - (int)tmp[2];
						dist = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
						if (dist < *pR)
						{
							++count;
						}
						++index;
					}

					if (count >= m_min)
					{
						*pMask = 0;
						int updateRate = (*pB)>0 ? blinkTs : ts;
						int rdm = random(0, updateRate);
						if (rdm == 0)
						{
							rdm = random(0, IWonder_MODEL_SIZE);
							backgroundModelsDownSample[rdm].at<Vec3b>(i, j) = pFrameDownSample.at<Vec3b>(i, j);
						}
						rdm = random(0, updateRate);
						if (rdm == 0)
						{
							int x = 0;
							int y = 0;
							while (x == 0 && y == 0)
							{
								x = random((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
								y = random((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
							}
							x += i;
							y += j;
							if (x < 0)
							{
								x = 0;
							}
							else if (x >= frameSizeDownSample.height)
							{
								x = frameSizeDownSample.height - 1;
							}
							if (y < 0)
							{
								y = 0;
							}
							else if (y >= frameSizeDownSample.width)
							{
								y = frameSizeDownSample.width - 1;
							}
							rdm = random(0, IWonder_MODEL_SIZE);
							backgroundModelsDownSample[rdm].at<Vec3b>(x, y) = pFrameDownSample.at<Vec3b>(i, j);
						}
					}
					else
					{
						*pMask = 255;
					}
				}
			}
		}
	}
	resize(maskDownSample, maskTmp, frameSize, 0, 0, CV_INTER_NN);
	bitwise_and(maskTmp, mask, fgmask);
	bitwise_xor(mask, maskTmp, pBlink);
	resize(mask, maskTmp, frameSizeDownSample, 0, 0, CV_INTER_NN);
	bitwise_xor(maskTmp, maskDownSample, pBlinkDownSample);
	updateR();
	if (countNonZero(fgmask) > beta * frameSize.area())
	{
		needRebuiltModel = true;
	}
	//mask.copyTo(fgmask);
	modifyMask(fgmask);
}

void CIWonder::getBackgroundImage(cv::Mat& image)
{
	//backgroundModels[0].copyTo(image);
	//maskDownSample.copyTo(image);
	mask.copyTo(image);
}

void CIWonder::init()
{
	srand((int)time(NULL));
	frameSize = pFrame.size();
	frameSizeDownSample = pFrameDownSample.size();
	for (int i = 0; i < IWonder_MODEL_SIZE; ++i)
	{
		backgroundModels[i].create(frameSize, CV_8UC3);
		backgroundModelsDownSample[i].create(frameSizeDownSample, CV_8UC3);
	}
	R.create(frameSize, CV_32FC1);
	R = Scalar::all(RThreshold);
	RDownSample.create(frameSizeDownSample, CV_32FC1);
	RDownSample = Scalar::all(RThreshold);
	pBlink.create(frameSize, CV_8UC1);
	pBlinkDownSample.create(frameSizeDownSample, CV_8UC1);
	initBackgroundModels();
}

void CIWonder::initBackgroundModels()
{

	RNG rng(getTickCount());
	int x = 0, y = 0;
	Vec3b tmp;
	for (int k = 0; k < IWonder_MODEL_SIZE; ++k)
	{
		for (int i = 0; i < frameSize.height; ++i)
		{
			uchar* pBM = backgroundModels[k].ptr<uchar>(i);
			for (int j = 0; j < frameSize.width; ++j)
			{

				x = i + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
				if (x < 0)
				{
					x = 0;
				}
				else if (x >= frameSize.height)
				{
					x = frameSize.height - 1;
				}

				y = j + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
				if (y < 0)
				{
					y = 0;
				}
				else if (y >= frameSize.width)
				{
					y = frameSize.width - 1;
				}
				tmp = pFrame.at<Vec3b>(x, y);
				pBM[0] = tmp[0];
				pBM[1] = tmp[1];
				pBM[2] = tmp[2];
				pBM += 3;
			}
		}

	}

	for (int k = 0; k < IWonder_MODEL_SIZE; ++k)
	{
		for (int i = 0; i < frameSizeDownSample.height; ++i)
		{
			uchar* pBM = backgroundModelsDownSample[k].ptr<uchar>(i);
			for (int j = 0; j < frameSizeDownSample.width; ++j)
			{

				x = i + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
				if (x < 0)
				{
					x = 0;
				}
				else if (x >= frameSizeDownSample.height)
				{
					x = frameSizeDownSample.height - 1;
				}

				y = j + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
				if (y < 0)
				{
					y = 0;
				}
				else if (y >= frameSizeDownSample.width)
				{
					y = frameSizeDownSample.width - 1;
				}
				tmp = pFrameDownSample.at<Vec3b>(x, y);
				pBM[0] = tmp[0];
				pBM[1] = tmp[1];
				pBM[2] = tmp[2];
				pBM += 3;
			}
		}

	}

}

int CIWonder::random(int a, int b)
{
	return rand() % (b - a) + a;
}

void CIWonder::updateR()
{
	for (int i = 0; i < frameSize.height; ++i)
	{
		float* pR = R.ptr<float>(i);
		uchar* pB = pBlink.ptr<uchar>(i);
		for (int j = 0; j < frameSize.width; ++j, ++pR, ++pB)
		{
			if (*pB > 0)
			{
				*pB += incR;
			}
			else
			{
				*pB = min(RThreshold, *pB + decR);
			}
		}
	}

	//downsample
	for (int i = 0; i < frameSizeDownSample.height; ++i)
	{
		float* pR = RDownSample.ptr<float>(i);
		uchar* pB = pBlinkDownSample.ptr<uchar>(i);
		for (int j = 0; j < frameSizeDownSample.width; ++j, ++pR, ++pB)
		{
			if (*pB > 0)
			{
				*pB += incR;
			}
			else
			{
				*pB = min(RThreshold, *pB + decR);
			}
		}
	}
}

void CIWonder::modifyMask(Mat& mask)
{
	//morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	//erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 7);
}
