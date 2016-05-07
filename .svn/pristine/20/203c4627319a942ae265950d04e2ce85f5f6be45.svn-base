#include "stdafx.h"
#include "MultiscaleSTBG.h"


CMultiscaleSTBG::CMultiscaleSTBG() :N(30), T1(20), T2(0.5), level(2), firstTime(true), Rthreshold(20)
{
	BGModel.resize(level + 1);
	for (int i = 0; i < level + 1; ++i )
	{
		BGModel[i].resize(N);
	}
}


CMultiscaleSTBG::~CMultiscaleSTBG()
{
}

void CMultiscaleSTBG::operator()(const cv::Mat& image, cv::Mat& fgmask)
{
	image.copyTo(pFrame);
	if (pFrame.channels() == 3)
	{
		cvtColor(pFrame, pFrame, CV_BGR2GRAY);
	}
	if (firstTime)
	{
		init();
		firstTime = false;
	}
	int x, y;
	uchar* p;
	uchar* ptrForeground;
	pFramePyr = getPyr(pFrame, level);
	double tmpProble = 0;
	//do
	//////////////////////////////////////////////////////////////////////////
	probleMap.release();
	probleMap.create(BGModel[level][0].size(), CV_32FC1);
	probleMap = Scalar::all(1);
	for (int k = level; k >= 0; --k)
	{
		Size size = BGModel[k][0].size();
		for (size_t ki = 0; ki < size.height; ki++)
		{
			float* p = probleMap.ptr<float>(ki);
			for (size_t kj = 0; kj < size.width; kj++, p++)
			{
				*p = *p * getPointProbability(ki, kj, pFramePyr[k], BGModel[k]);
			}
		}
		updateBGModel(k);
		if (k > 0)
		{
			resize(probleMap, probleMap, BGModel[k - 1][0].size(), CV_INTER_NN);
		}
	}
	getforeMask(fgmask);
	//////////////////////////////////////////////////////////////////////////
	//for (size_t i = 0; i < pFrame.rows; i++)
	//{
	//	ptrForeground = foreground.ptr<uchar>(i);
	//	for (size_t j = 0; j < pFrame.cols; j++)
	//	{
	//		double proble = 1;	
	//		for (size_t k = 0; k < level + 1; k++)
	//		{
	//			Size size = BGModel[k][0].size();
	//			x = static_cast<int>(1.0 * size.height / pFrame.rows * i);
	//			y = static_cast<int>(1.0 * size.width / pFrame.cols * j);
	//			tmpProble = getPointProbability(x, y, pFramePyr[k], BGModel[k]);
	//			proble *= tmpProble;
	//			updateLevelBGModel(x, y, proble, k);
	//		}
	//		*ptrForeground++ = proble >= T2 ? 255 : 0;
	//		//update
	//		//updateBGModel(i, j, proble);
	//	}
	//}
	//foreground.copyTo(fgmask);
}

void CMultiscaleSTBG::getBackgroundModel(cv::Mat& image)
{
	//tmple
	//pFrame.copyTo(image);
	Mat res;
	res.create(pFrame.size(), CV_32SC1);
	Mat tmp;
	res = Scalar::all(0);
	for (int i = 0; i < N; ++i)
	{
		BGModel[0][i].convertTo(tmp, CV_32S);
		res += tmp;
	}
	res.convertTo(image, CV_8U, 1.0 / N, 0);
	putText(image, ("this is first Pyramid mean of bgmodel "), Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0, 0, 0), 2.0);
}

void CMultiscaleSTBG::init()
{
	foreground.create(pFrame.size(), CV_8UC1);
	foreground = Scalar::all(0);
	initBGModel();
}

vector<Mat> CMultiscaleSTBG::getPyr(const Mat& src, int level /*= 3*/)
{
	vector<Mat> res;
	buildPyramid(src, res, level);
	return res;
}

void CMultiscaleSTBG::initBGModel()
{
	vector<Mat> res = getPyr(pFrame, level);
	for (int i = 0; i < level + 1; ++i)
	{
		initLevelBGModel(i, res[i], res[i].size());
	}
}

void CMultiscaleSTBG::initLevelBGModel(int level, const Mat& src, Size size)
{
	int x, y;
	Point pos;
	uchar* pBG;
	uchar* p = src.data;
	for (int index = 0; index < N; ++index)
	{
		BGModel[level][index].create(size, CV_8UC1);
		BGModel[level][index] = Scalar::all(0);
		for (size_t i = 0; i < size.height; i++)
		{
			pBG = BGModel[level][index].ptr<uchar>(i);
			for (size_t j = 0; j < size.width; j++)
			{
				pos = getRandomXY(Point(i, j), size);
				*pBG++ = src.at<uchar>(pos.x,  pos.y);
				//*pBG = p[pos.x * src.step[0] + pos.y];
			}

		}
	}
}

Point CMultiscaleSTBG::getRandomXY(Point pos, Size size)
{
	pos.x = pos.x + rng.uniform(-1, 2);
	if (pos.x < 0)
	{
		pos.x = -pos.x;
	}
	else if (pos.x >= size.height)
	{
		pos.x = 2 * (size.height - 1) - pos.x;
	}

	pos.y = pos.y + rng.uniform(-1, 2);
	if (pos.y < 0)
	{
		pos.y = -pos.y;
	}
	else if (pos.y >= size.width)
	{
		pos.y = 2 * (size.width - 1) - pos.y;
	}
	return pos;
}

double CMultiscaleSTBG::getPointProbability(int x, int y, const Mat& frame, const vector<Mat>& bgmodel)
{
	int count = 0;
	for (size_t i = 0; i < bgmodel.size(); i++)
	{
		if (std::abs(bgmodel[i].at<uchar>(x, y) - frame.at<uchar>(x, y)) > Rthreshold)
		{
			count++;
		}
	}
	return 1.0 * count / bgmodel.size();
}


void CMultiscaleSTBG::updateBGModel(int level)
{
	Size size = BGModel[level][0].size();
	CV_Assert(size == probleMap.size());
	double p;
	float* pMap;
	int rdm = 0; 
	for (size_t i = 0; i < size.height; i++)
	{
		pMap = probleMap.ptr<float>(i);
		for (size_t j = 0; j < size.width; j++)
		{
			p = rng.uniform((double)0.0, (double)1.0);
			if (p < (1 - pMap[j]))
			{
				rdm = rng.uniform(0, N);
				BGModel[level][rdm].at<uchar>(i, j) = pFramePyr[level].at<uchar>(i, j);
			}
		}
	}
}

void CMultiscaleSTBG::updateLevelBGModel(int x, int y, double proba, int level)
{
	double p = rng.uniform((double)0.0, (double)1.0);
	if (p < (1 - proba))
	{
		int rdm = rng.uniform(0, N);
		BGModel[level][rdm].at<uchar>(x, y) = pFramePyr[level].at<uchar>(x, y);
	}
	else
		return;
}

void CMultiscaleSTBG::getforeMask(Mat& img)
{
	CV_Assert(probleMap.size() == pFrame.size());
	img.create(pFrame.size(), CV_8UC1);
	img = Scalar::all(0);
	uchar* p;
	float* pProble;
	for (int i = 0; i < img.rows; ++i)
	{
		p = img.ptr<uchar>(i);
		pProble = probleMap.ptr<float>(i);
		for (int j = 0; j < img.cols; ++j, ++p, ++pProble)
		{
			*p = *pProble >= T2 ? 255 : 0;
		}
	}
}
