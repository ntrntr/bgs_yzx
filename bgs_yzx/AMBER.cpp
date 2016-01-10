#include "stdafx.h"
#include "AMBER.h"


CAMBER::CAMBER() :T_Y_min(17.5)
, T_Y_max(80.0)
, T_Cb_min(9)
, T_Cb_max(12)
, T_Cr_min(9)
, T_Cr_max(12)
, deltaY(2.5)
, deltaCb(1)
, deltaCr(1)
, alpha(0.01)
, thetaH(400)
, thetaL(2500)
, gamma(1.5)
, L0(6000)
, L1(4000)
, betaMax(75)
, betaInc(6)
, betaDec(1)
, betaTH(60)
, nMax(0.03)
, nMin(0.0004)
, K(5)
, firsttime(true)
, downSampleUpdateRate(16)
, distanceFactor(5.0)
, forehitT(1)
{
	Tycbcr[0] = 20.0;
	Tycbcr[1] = 10;
	Tycbcr[2] = 10;
}


CAMBER::~CAMBER()
{
}

void CAMBER::init()
{
}

void CAMBER::colorJudgeAndUpdata(int x, int y, int level)
{
	if (level == 0)
	{
		int idx = x * m_oImgSize.width + y;
		Vec3b p = pFrame.at<Vec3b>(x, y);
		int isBackground = 0;
		for (int i = 0; i < term[idx].size(); ++i)
		{
			if (!isBackground && term[idx][i].count > 0 && abs(term[idx][i].data[0] - p[0]) < Tycbcr[0] &&
				abs(term[idx][i].data[1] - p[1]) < Tycbcr[1] && 
				abs(term[idx][i].data[2] - p[2]) < Tycbcr[2])
			{
				if (!isBackground)
				{
					isBackground = 1;
					foregroundMask.at<uchar>(x, y) = 0;
				}
				++term[idx][i].count;
				updateBk(term[idx][i].data, p);
			}
			else
			{
				term[idx][i].count--;
			}
		}
		term[idx][0].count = min(term[idx][0].count, L0);
		if (term[idx].size()> 1)
		{
			term[idx][1].count = min(term[idx][1].count, L1);
		}
		//short-term template ordering
		if (term[idx].size() > 1)
		{
			sort(term[idx].begin() + 1, term[idx].end(), [](const node& a, const node& b)
			{
				return a.count > b.count;
			});
		}
		//template replacement
		if (!isBackground)
		{
			int hitcount = 0;
			for (int i = 0; i < TA[idx].size(); ++i)
			{
				if (abs(TA[idx][i].data[0] - p[0]) < Tycbcr[0] &&
					abs(TA[idx][i].data[1] - p[1]) < Tycbcr[1] &&
					abs(TA[idx][i].data[2] - p[2]) < Tycbcr[2])
				{
					hitcount++;
					TA[idx][i].count += (ceil)(
						((Tycbcr[0] - abs(TA[idx][i].data[0] - p[0])) / Tycbcr[0] +
						(Tycbcr[1] - abs(TA[idx][i].data[1] - p[1])) / Tycbcr[1] +
						(Tycbcr[2] - abs(TA[idx][i].data[2] - p[2])) / Tycbcr[2]) * distanceFactor + 1
						);
				}
				else
				{
					TA[idx][i].count--;
				}
			}
			if (hitcount <= forehitT)
			{
				TA[idx].push_back(node(p, 1));
			}
			sort(TA[idx].begin(), TA[idx].end(), [](const node& a, const node& b)
			{
				return a.count > b.count;
			});
			TA[idx].erase(lower_bound(TA[idx].begin(), TA[idx].end(), node(p, -300)), TA[idx].end());
			if (TA[idx].size() > 0 && TA[idx][0].count >thetaH)
			{
				if (term[idx].size()-1 < K)
				{
					term[idx].push_back(TA[idx][0]);
					TA[idx].erase(TA[idx].begin());
				}
				else if (term[idx][term[idx].size() - 1].count <= 0)
				{
					term[idx].pop_back();
					term[idx].push_back(TA[idx][0]);
					TA[idx].erase(TA[idx].begin());
				}
			}
		}
		//long term template update
		if (term[idx].size() >= 2 && term[idx][0].count < thetaL && term[idx][1].count > thetaL)
		{
			swap(term[idx][0], term[idx][1]);
			term[idx][0].count = (int)(gamma * thetaL);
		}
	}
	if (level == 1)
	{
		int idx = x * m_oImgSizeDownSample.width + y;
		Vec3b p = pFrameDownSample.at<Vec3b>(x, y);
		int isBackground = 0;
		for (int i = 0; i < termDownSample[idx].size(); ++i)
		{
			if (!isBackground && termDownSample[idx][i].count && abs(termDownSample[idx][i].data[0] - p[0]) < Tycbcr[0] &&
				abs(termDownSample[idx][i].data[1] - p[1]) < Tycbcr[1] && 
				abs(termDownSample[idx][i].data[2] - p[2]) < Tycbcr[2])
			{
				if (!isBackground)
				{
					foregroundMaskDownSample.at<uchar>(x, y) = 0;
					isBackground = 1;
				}
				//updateBk(termDownSample[idx][i].data, p);
				++termDownSample[idx][i].count;
			}
			else
			{
				--termDownSample[idx][i].count;
			}
		}
		termDownSample[idx][0].count = min(termDownSample[idx][0].count, L0);
		if (termDownSample[idx].size() > 1)
		{
			termDownSample[idx][1].count = min(termDownSample[idx][1].count, L1);
		}

		//short-term template ordering
		if (termDownSample[idx].size() > 1)
		{
			sort(termDownSample[idx].begin() + 1, termDownSample[idx].end(), [](const node& a, const node& b)
			{
				return a.count > b.count;
			});
		}
		//template replacement
		if (!isBackground)
		{
			int hitcount = 0;
			for (int i = 0; i < TADOWNSAMPLE[idx].size(); ++i)
			{
				if (abs(TADOWNSAMPLE[idx][i].data[0] - p[0]) < Tycbcr[0] &&
					abs(TADOWNSAMPLE[idx][i].data[1] - p[1]) < Tycbcr[1] &&
					abs(TADOWNSAMPLE[idx][i].data[2] - p[2]) < Tycbcr[2])
				{
					hitcount++;
					//TADOWNSAMPLE[idx][i].count++;
					TADOWNSAMPLE[idx][i].count += (ceil)(
						((Tycbcr[0] - abs(TADOWNSAMPLE[idx][i].data[0] - p[0])) / Tycbcr[0] +
						(Tycbcr[1] - abs(TADOWNSAMPLE[idx][i].data[1] - p[1])) / Tycbcr[1] +
						(Tycbcr[2] - abs(TADOWNSAMPLE[idx][i].data[2] - p[2])) / Tycbcr[2]) * distanceFactor + 1
						);
				}
				else
				{
					TADOWNSAMPLE[idx][i].count--;
				}
			}
			if (hitcount <= forehitT)
			{
				TADOWNSAMPLE[idx].push_back(node(p, 1));
			}
			sort(TADOWNSAMPLE[idx].begin(), TADOWNSAMPLE[idx].end(), [](const node& a, const node& b)
			{
				return a.count > b.count;
			});
			TADOWNSAMPLE[idx].erase(lower_bound(TADOWNSAMPLE[idx].begin(), TADOWNSAMPLE[idx].end(), node(p, -300)), TADOWNSAMPLE[idx].end());
			if (TADOWNSAMPLE[idx].size() > 0 && TADOWNSAMPLE[idx][0].count > thetaH)
			{
				if (termDownSample[idx].size() - 1 < K)
				{
					termDownSample[idx].push_back(TADOWNSAMPLE[idx][0]);
					TADOWNSAMPLE[idx].erase(TADOWNSAMPLE[idx].begin());
				}
				else if (termDownSample[idx][termDownSample[idx].size() - 1].count <= 0)
				{
					termDownSample[idx].pop_back();
					termDownSample[idx].push_back(TADOWNSAMPLE[idx][0]);
					TADOWNSAMPLE[idx].erase(TADOWNSAMPLE[idx].begin());
				}
			}
		}
		//long term template update
		if (termDownSample[idx].size() >= 2 && termDownSample[idx][0].count < thetaL && termDownSample[idx][1].count > thetaL)
		{
			swap(termDownSample[idx][0], termDownSample[idx][1]);
			termDownSample[idx][0].count = (int)(gamma * thetaL);
		}
	}
	
}

void CAMBER::updateBk(double* bk, Vec3b& p)
{
	bk[0] = (1.0 - alpha) * bk[0] + alpha * p[0];
	bk[1] = (1.0 - alpha) * bk[1] + alpha * p[1];
	bk[2] = (1.0 - alpha) * bk[2] + alpha * p[2];
}

void CAMBER::getForegroundMask(Mat& fgmask)
{
	Mat tmp;
	cv::dilate(foregroundMaskDownSample, tmp, cv::Mat(), Point(0, 0), 1);
	cv::resize(tmp, tmp, foregroundMask.size(), 0, 0, CV_INTER_NN);
	cv::bitwise_and(foregroundMask, tmp, fgmask);
	cv::bitwise_and(fgmask, finalMask, fgmask);
	//foregroundMask.copyTo(fgmask);
	//foregroundMaskDownSample.copyTo(fgmask);
}

void CAMBER::postProcessAndGlobalControlProcedure()
{
	finalMask.create(m_oImgSize, CV_8UC1);
	finalMask = Scalar::all(255);
	cv::dilate(foregroundMaskDownSample, noiseMap, cv::Mat(), Point(0, 0), 1);
	cv::resize(noiseMap, noiseMap, foregroundMask.size(), 0, 0, CV_INTER_NN);
	//pyrUp(tmp, tmp, foregroundMask.size());
	//cv::bitwise_xor(foregroundMask, tmp, tmp);
	cv::bitwise_and(foregroundMask, noiseMap, noiseMap);
	//tmp.copyTo(noiseMap);
	cv::bitwise_xor(prevnoiseMap, noiseMap, blinkingMap);
	//cv::bitwise_and(foregroundMask, tmp, noiseMap);
	//cv::bitwise_xor(noiseMap, prevnoiseMap, tmp);
	int count = countNonZero(noiseMap);
	for (int i = 0; i < blinkingMap.rows; ++i)
	{
		uchar* pnoise = blinkingMap.ptr<uchar>(i);
		uchar* pfinalMask = finalMask.ptr<uchar>(i);
		int* p = An.ptr<int>(i);

		for (int j = 0; j < blinkingMap.cols; ++j, ++pnoise, ++p, ++pfinalMask)
		{
			if (*pnoise > 0)
			{
				*p += betaInc;
				*p = min(*p, betaMax);
			}
			else
			{
				*p -= betaDec;
				*p = max(0, *p);
			}
			if (*p > betaTH)
			{
				*pfinalMask = 0;
			}
		}
	}
	if (count > (int)(nMax * m_oImgSize.area()))
	{
		Tycbcr[0] = min(Tycbcr[0] + deltaY, T_Y_max);
		Tycbcr[1] = min(Tycbcr[1] + deltaCb, T_Cb_max);
		Tycbcr[2] = min(Tycbcr[2] + deltaCr, T_Cr_max);
	}
	else if (count < (int)(nMin * m_oImgSize.area()))
	{
		Tycbcr[0] = max(Tycbcr[0] - deltaY, T_Y_min);
		Tycbcr[1] = max(Tycbcr[1] - deltaCb, T_Cb_min);
		Tycbcr[2] = max(Tycbcr[2] - deltaCr, T_Cr_min);
	}
	noiseMap.copyTo(prevnoiseMap);
}

void CAMBER::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate /*= 0*/)
{
	cvtColor(image, pFrame, CV_BGR2YCrCb);
	//image.copyTo(pFrame);
	pyrDown(pFrame, pFrameDownSample);
	if(firsttime)
	{
		m_oImgSize = pFrame.size();
		m_oImgSizeDownSample = pFrameDownSample.size();
		totalCount = pFrame.rows * pFrame.cols;
		totalCountDownSample = pFrameDownSample.rows * pFrameDownSample.cols;
		term.resize(totalCount);
		termDownSample.resize(totalCountDownSample);
		for (int i = 0; i < pFrame.rows; ++i)
		{
			for (int j = 0; j < pFrame.cols; ++j)
			{
				term[i * pFrame.cols + j].push_back(node(pFrame.at<Vec3b>(i, j), (int)(thetaH)));
			}
		}
		for (int i = 0; i < pFrameDownSample.rows; ++i)
		{
			for (int j = 0; j < pFrameDownSample.cols; ++j)
			{
				termDownSample[i * pFrameDownSample.cols + j].push_back(node(pFrameDownSample.at<Vec3b>(i, j), (int)(thetaH)));
			}
		}
		TA.resize(totalCount);
		TADOWNSAMPLE.resize(totalCountDownSample);
		prevnoiseMap.create(m_oImgSize, CV_8UC1);
		prevnoiseMap = Scalar::all(0);
		An.create(m_oImgSize, CV_32SC1);
		An = Scalar::all(0);
		init();
		firsttime = false;
	}
	foregroundMask.create(pFrame.size(), CV_8UC1);
	foregroundMask = Scalar::all(255);
	foregroundMaskDownSample.create(pFrameDownSample.size(), CV_8UC1);
	foregroundMaskDownSample = Scalar::all(255);
#pragma omp parallel sections
	{
#pragma omp section
		{
			for (int i = 0; i < pFrame.rows; ++i)
			{
				for (int j = 0; j < pFrame.cols; ++j)
				{
					colorJudgeAndUpdata(i, j, 0);
				}
			}
		}
#pragma omp section
		{
			for (int i = 0; i < pFrameDownSample.rows; ++i)
			{
				for (int j = 0; j < pFrameDownSample.cols; ++j)
				{
					colorJudgeAndUpdata(i, j, 1);
				}
			}
		}
		
	}
	
	postProcessAndGlobalControlProcedure();
	getForegroundMask(fgmask);
	
}

void CAMBER::getBackgroundImage(cv::Mat& image)
{
	image.create(m_oImgSize, CV_32FC3);
	//if (image.isContinuous())
	//{
	//	for (int i = 0; i < m_oImgSize.area(); ++i)
	//	{
	//		for (int c = 0; c < 3; ++c)
	//		{
	//			image.data[i * 3 + c] = (float)term[i][0].data[c];
	//		}
	//	}
	//}
	
//#pragma omp parallel for
	for (int i = 0; i < image.rows; ++i)
	{
		Vec3f* p = image.ptr<Vec3f>(i);
		for (int j = 0; j < image.cols; ++j, ++p)
		{
			(*p)[0] = term[i * image.cols + j][0].data[0];
			(*p)[1] = term[i * image.cols + j][0].data[1];
			(*p)[2] = term[i * image.cols + j][0].data[2];
		}
	}
	image.convertTo(image, CV_8U);
	cvtColor(image, image, CV_YCrCb2BGR);
}


