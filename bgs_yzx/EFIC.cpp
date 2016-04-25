#include "stdafx.h"
#include "EFIC.h"
#include "LTPS.h"
#include <amp_math.h>
static const int M = 5;
static const double T_e = 0.12;
static const double T_w = 0.85;
static const double a_c = 0.0005;
static const double Tau = 0.04;
static const double mu = 0.2;
static const double theta = 0.05;
static const double C_i = 0.05;
static const double T_d = 3;
static const double T_f = 1.0;
static const double T_n = 0.7;
TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
Size subPixWinSize(10, 10), winSize(31, 31);

const int MAX_COUNT = 500;
bool needToInit = true;
bool nightMode = false;
Point2f point;
bool addRemovePt = false;
CEFIC::CEFIC() :isinit(false), exponentially_t(1)
{
	termcrit = TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
	points.resize(3);
}


CEFIC::~CEFIC()
{
}

void CEFIC::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate /*= 0*/)
{
	if (image.channels() == 3)
	{
		cvtColor(image, pFrame, CV_BGR2GRAY);
	}
	GaussianBlur(pFrame, pFrame, cv::Size(3, 3), 0);
	
	eficForegroundMask.create(pFrame.size(), CV_8UC1);
	eficForegroundMask = cv::Scalar::all(0);
	LTPs.create(pFrame.size(), CV_32FC2);
	LTPs = cv::Scalar::all(0);
	doltp();
	ltps(pFrame, pFrame.rows, pFrame.cols, 1, LTPs);
	if (!isinit)
	{
		backgroundModel = vector<vector<vector<ltps_node>>>(pFrame.rows, vector<vector<ltps_node>>(pFrame.cols));
		maxgradientlen = vector<vector<double>>(pFrame.rows, vector<double>(pFrame.cols,5.0));
		initBackgroundModel();
		updateRate.create(pFrame.size(), CV_32FC1);
		updateRate = cv::Scalar::all(1.0/M);
		//globalUnreliability.create(pFrame.size(), CV_32FC1);
		//globalUnreliability = cv::Scalar::all(0);
		localUnreliability.create(pFrame.size(), CV_32FC1);
		localUnreliability = cv::Scalar::all(0);
		//alpha_i_t.create(pFrame.size(), CV_32FC1);
		//alpha_i_t = cv::Scalar::all(0);
		prevEficForegroundMask.create(pFrame.size(), CV_8UC1);
		prevEficForegroundMask = cv::Scalar::all(0);
		pFrame.copyTo(prevFrame);
		isinit = true;
		
	}
	if (needToInit)
	{
		// automatic initialization
		goodFeaturesToTrack(pFrame, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
		cornerSubPix(pFrame, points[1], subPixWinSize, Size(-1, -1), termcrit);
		addRemovePt = false;
	}
	opticalFlowFarneback();
	if (countMovePoint() > T_n)
	{
		//motion detected
		image.copyTo(LKimage);
		OpticalFlowPyrLK();
	}
	updateBackgroundModel();
	updateLearningRate();
	Mat gradientLTP;
	gradientLTP.create(pFrame.size(), CV_32FC1);
	ltpsTogradient(LTPs, gradientLTP);
	cv::normalize(gradientLTP, gradientLTP, 1, 0, NORM_MINMAX, CV_32FC1);
	cv::erode(eficForegroundMask, eficForegroundMask, cv::Mat(), cv::Point(0, 0), 1);
	LKimage.copyTo(fgmask);
	//refineSegments(eficForegroundMask, fgmask);
	//eficForegroundMask.copyTo(fgmask);
	
	//cflow.copyTo(fgmask);
	exponentially_t++;
	eficForegroundMask.copyTo(prevEficForegroundMask);
	pFrame.copyTo(prevFrame);
}

void CEFIC::updateBackgroundModel()
{
	float* pltps;
	uchar* pmask;
	for (int i = 0; i < backgroundModel.size(); ++i)
	{
		pltps = LTPs.ptr<float>(i);
		pmask = eficForegroundMask.ptr<uchar>(i);
		for (int j = 0; j < backgroundModel[i].size(); ++j)
		{
			int idx = -1;
			double minsquart = 10000.0;
			double tmp = 0;
			int count = 0;
			for (int k = 0; k < backgroundModel[i][j].size(); ++k)
			{
				tmp = getDistance(backgroundModel[i][j][k], pltps);
				if (tmp > T_e * maxgradientlen[i][j])
				{
					count++;
				}
				if (tmp < minsquart)
				{
					idx = k;
					minsquart = tmp;
				}
			}
			if (minsquart > T_e * maxgradientlen[i][j])
			{
				*pmask = 255;
			}
			if (count == backgroundModel[i][j].size() && backgroundModel[i][j].size() < M)
			{
				backgroundModel[i][j].push_back(ltps_node(pltps[0], pltps[1]));	
				maxgradientlen[i][j] = std::max(maxgradientlen[i][j], (double)sqrt(pltps[0] * pltps[0] + pltps[1] * pltps[1]));
			}
			else if (count == M)
			{
				backgroundModel[i][j][M - 1] = ltps_node(pltps[0], pltps[1], backgroundModel[i][j][M - 1].weight);
				maxgradientlen[i][j] = std::max(maxgradientlen[i][j], (double)sqrt(pltps[0] * pltps[0] + pltps[1] * pltps[1]));
			}
			
			//update v

			updateV_k(backgroundModel[i][j][idx], pltps[0], pltps[1], updateRate.at<float>(i, j));
			for (int k = 0; k < backgroundModel[i][j].size(); ++k)
			{
				if (k != idx)
				{
					backgroundModel[i][j][k].weight = (1 - updateRate.at<float>(i, j)) *  backgroundModel[i][j][k].weight;
				}
				else
				{
					backgroundModel[i][j][k].weight = updateRate.at<float>(i, j) + (1 - updateRate.at<float>(i, j)) *  backgroundModel[i][j][k].weight;
				}
			}
			sort(backgroundModel[i][j].begin(), backgroundModel[i][j].end());
			pltps += 2;
			pmask++;
		}
	}
}

double CEFIC::getDistance(ltps_node& a, const float* p)
{
	return sqrt((a.x - p[0])*(a.x - p[0]) + (a.y - p[1]) * (a.y - p[1]));
	//return ((a.x - p[0])*(a.x - p[0]) + (a.y - p[1]) * (a.y - p[1]));
}

void CEFIC::updateLearningRate()
{
	float* p;
	double exponentiallyRate = getExponentiallyRate(exponentially_t);
	double m_alpha_i_t = C_i + (1 - C_i) * M * exponentiallyRate;
	float U_G_t = 0.5 / M * (1 + Concurrency::precise_math::erf(
		(countNonZero(eficForegroundMask)*1.0 / (eficForegroundMask.rows * eficForegroundMask.cols) - mu)
		/ theta));
	float* p_local;
	for (int i = 0; i < localUnreliability.rows; ++i)
	{
		p_local = localUnreliability.ptr<float>(i);
		for (int j = 0; j < localUnreliability.cols; ++j)
		{
			(*p_local) = (m_alpha_i_t)* getRcurr(i, j) + (1 - m_alpha_i_t) * getRprev(i, j);
			++p_local;
		}
	}
	GaussianBlur(localUnreliability, localUnreliability, Size(3, 3), 0);
	for (int i = 0; i < updateRate.rows; ++i)
	{
		p = updateRate.ptr<float>(i);
		p_local = localUnreliability.ptr<float>(i);
		for (int j = 0; j < updateRate.cols; ++j)
		{
			
			(*p) = std::min(1.0 / M, a_c + exponentiallyRate + std::max((*p_local), U_G_t));
			p++;
			p_local++;
		}
	}
}

double CEFIC::getExponentiallyRate(int t)
{
	return 1.0 / M * std::pow(2.718281828, -1.0 *  Tau * t);
}

void CEFIC::updateV_k(ltps_node& a, double x, double y, double rate)
{
	a.x = rate * x + (1 - rate) * a.x;
	a.y = rate * y + (1 - rate) * a.y;
}

int CEFIC::getRcurr(int x, int y)
{
	if (x == 0 || y == 0 || x == eficForegroundMask.rows - 1 || y == eficForegroundMask.cols - 1)
	{
		return 0;
	}
	if (eficForegroundMask.at<uchar>(x, y) != 255)
	{
		return 0;
	}
	if (eficForegroundMask.at<uchar>(x, y) == 255
		&& eficForegroundMask.at<uchar>(x - 1, y - 1) == 0
		&& eficForegroundMask.at<uchar>(x, y - 1) == 0
		&& eficForegroundMask.at<uchar>(x + 1, y - 1) == 0
		&& eficForegroundMask.at<uchar>(x - 1, y) == 0
		&& eficForegroundMask.at<uchar>(x + 1, y) == 0
		&& eficForegroundMask.at<uchar>(x - 1, y + 1) == 0
		&& eficForegroundMask.at<uchar>(x, y + 1) == 0
		&& eficForegroundMask.at<uchar>(x + 1, y + 1) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CEFIC::getRprev(int x, int y)
{
	if (x == 0 || y == 0 || x == prevEficForegroundMask.rows - 1 || y == prevEficForegroundMask.cols - 1)
	{
		return 0;
	}
	if (prevEficForegroundMask.at<uchar>(x,y) != 255)
	{
		return 0;
	}
	if (prevEficForegroundMask.at<uchar>(x, y) == 255
		&& prevEficForegroundMask.at<uchar>(x-1, y-1) == 0
		&& prevEficForegroundMask.at<uchar>(x, y-1) == 0
		&& prevEficForegroundMask.at<uchar>(x+1,y-1) == 0
		&& prevEficForegroundMask.at<uchar>(x-1, y) == 0
		&& prevEficForegroundMask.at<uchar>(x+1, y) == 0
		&& prevEficForegroundMask.at<uchar>(x-1, y+1) == 0
		&& prevEficForegroundMask.at<uchar>(x, y+1) == 0
		&& prevEficForegroundMask.at<uchar>(x+1, y+1) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CEFIC::refineSegments(Mat& mask, Mat& dst)
{
	int niters = 3;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours0;
	findContours(mask, contours0, hierarchy, /*CV_RETR_CCOMP*/RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	dst = Mat::zeros(mask.size(), CV_8UC3);
	if (contours0.size() == 0)
	{
		return;
	}
	contours.resize(contours0.size());
	for (size_t k = 0; k < contours0.size(); k++)
		approxPolyDP(Mat(contours0[k]), contours[k], 3, true);
	int idx = 0, largestComp = 0;
	double maxArea = 0;

	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		const vector<Point>& c = contours[idx];
		double area = fabs(contourArea(Mat(c)));
		//if (area > maxArea)
		//{
		//	maxArea = area;
		//	largestComp = idx;
		//}
		if (area < 10)
		{
			continue;
		}
		//Scalar color(rand() % 255, rand() % 255, rand() % 255);
		Scalar color(255, 255, 255);
		drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
	}
	/*Scalar color(0, 0, 255);
	drawContours(dst, contours, largestComp, color, CV_FILLED, 8, hierarchy);*/
}

void CEFIC::opticalFlowFarneback()
{
	
	calcOpticalFlowFarneback(prevFrame, pFrame, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
	//cvtColor(prevFrame, cflow, COLOR_GRAY2BGR);
	//drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(100, 255, 0));
}

void CEFIC::drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, double, const Scalar& color)
{
	for (int y = 0; y < cflowmap.rows; y += step)
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
				color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
}

double CEFIC::countMovePoint()
{
	int count = 0;
	float* p;
	for (int i = 0; i < flow.rows; ++i)
	{
		p = flow.ptr<float>(i);
		for (int j = 0; j < flow.cols; ++j)
		{
			if (sqrt (p[0]*p[0] + p[1]*p[1]) > T_f)
			{
				count++;
			}
			p += 2;
		}
	}
	return 1.0 * count / (flow.rows * flow.cols);
}

void CEFIC::OpticalFlowPyrLK()
{

	if (nightMode)
		LKimage = Scalar::all(0);
	if (needToInit)
	{
		// automatic initialization
		goodFeaturesToTrack(pFrame, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
		cornerSubPix(pFrame, points[1], subPixWinSize, Size(-1, -1), termcrit);
		addRemovePt = false;
		needToInit = false;
	}
	else if (!points[0].empty())
	{
		vector<uchar> status;
		vector<float> err;
		if (prevFrame.empty())
			pFrame.copyTo(prevFrame);
		calcOpticalFlowPyrLK(prevFrame, pFrame, points[0], points[1], status, err, winSize,
			3, termcrit, 0, 0.001);
		points[2].resize(points[0].size());
		size_t i, k;
		for (i = k = 0; i < points[1].size(); i++)
		{
			if (addRemovePt)
			{
				if (norm(point - points[1][i]) <= 5)
				{
					addRemovePt = false;
					continue;
				}
			}

			if (!status[i])
				continue;

			points[2][k] = points[0][i];
			points[1][k++] = points[1][i];
			
			//circle(LKimage, points[1][i], 2, Scalar(0, 255, 0));
			//line(LKimage, points[0][i], points[1][i], Scalar(0, 0, 255));
		}
		points[1].resize(k);
		points[2].resize(k);

		if (points[2].size() > 5)
		{
			H = findHomography(points[2], points[1], RANSAC, 4, match_mask);
			if (countNonZero(Mat(match_mask)) > 15)
			{
				H_prev = H;
			}
			else
				H_prev = Mat::eye(3, 3, CV_32FC1);

			drawMatchesPoint(LKimage, match_mask);
		}
		else
		{
			needToInit = true;
			H_prev = Mat::eye(3, 3, CV_32FC1);
		}
	}

	if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
	{
		vector<Point2f> tmp;
		tmp.push_back(point);
		cornerSubPix(pFrame, tmp, winSize, cvSize(-1, -1), termcrit);
		points[1].push_back(tmp[0]);
		addRemovePt = false;
	}
	
	//needToInit = false;
	//imshow("LK Demo", image);

	//char c = (char)waitKey(10);
	//if (c == 27)
	//	break;
	//switch (c)
	//{
	//case 'r':
	//	needToInit = true;
	//	break;
	//case 'c':
	//	points[0].clear();
	//	points[1].clear();
	//	break;
	//case 'n':
	//	nightMode = !nightMode;
	//	break;
	//}

	std::swap(points[1], points[0]);
	//cv::swap(prevFrame, pFrame);
}

void CEFIC::drawMatchesPoint(Mat& img, vector<unsigned char>& mask /*= vector< unsigned char>()*/)
{
	for (int i = 0; i < mask.size(); ++i)
	{
		if (mask[i])
		{
			circle(img, points[1][i], 2, Scalar(0, 255, 0));
			line(img, points[2][i], points[1][i], Scalar(0, 0, 255));
		}
	}
}

void CEFIC::saveBackgroundModels(cv::Mat& image)
{
	image.create(pFrame.size(), CV_32FC1);
	image = cv::Scalar::all(0);
	float* p;
	for (int i = 0; i < backgroundModel.size(); ++i)
	{
		p = image.ptr<float>(i);
		for (int j = 0; j < backgroundModel[i].size(); ++j)
		{
			*p = (float)sqrt(backgroundModel[i][j][0].x *  backgroundModel[i][j][0].x + backgroundModel[i][j][0].y * backgroundModel[i][j][0].y);
			p++;
		}
	}
	cv::normalize(image, image, 0, 255, NORM_MINMAX, CV_8UC1);
}

void CEFIC::createLTPs(cv::Mat& source, cv::Mat& dest)
{
}

void CEFIC::ltpsTogradient(cv::Mat& source, cv::Mat& dest)
{
	float* pmask;
	float* pltps;
	for (int i = 0; i < dest.rows; ++i)
	{
		pmask = dest.ptr<float>(i);
		pltps = source.ptr<float>(i);
		for (int j = 0; j < dest.cols; ++j)
		{
			*pmask = (float)(sqrt(pltps[0] * pltps[0] + pltps[1] * pltps[1]));

			pmask++;
			pltps += 2;
		}
	}
}

void CEFIC::doltp()
{
	calculate_points();
}

void CEFIC::initBackgroundModel()
{
	float* pltps;
	for (int i = 0; i < backgroundModel.size(); ++i)
	{
		pltps = LTPs.ptr<float>(i);
		for (int j = 0; j < backgroundModel[i].size(); ++j)
		{	
			backgroundModel[i][j].push_back(ltps_node(pltps[0], pltps[1]));
			maxgradientlen[i][j] = sqrt(pltps[0] * pltps[0] + pltps[1] * pltps[1]);
			pltps += 2;
		}
	}
}

