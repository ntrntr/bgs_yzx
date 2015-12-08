#include "stdafx.h"
#include "PBAS.h"


CPBAS::CPBAS() : m_min(2), R_inc_dec(0.05), R_lower(18), R_scale(5), T_dec(0.05), T_inc(1.0), T_lower(2), T_upper(200), alpha(10), firstTime(true), patchsize(3)
{
	//backgroundModel.resize(N);
	//R.resize(N);
	//D.resize(N);
	c_xoff.resize(patchsize);
	c_yoff.resize(patchsize);
	for (int tmp = -patchsize / 2, i = 0; i < patchsize; ++i, ++tmp)
	{
		c_xoff[i] = tmp;
		c_yoff[i] = tmp;
	}
}


CPBAS::~CPBAS()
{
}
void CPBAS::init()
{
	
	for (int i = 0; i < N; ++i)
	{
		backgroundModel[i].create(pFrame.size(), CV_32FC1);
		backgroundModel[i] = cv::Scalar::all(0);
		R[i].create(pFrame.size(), CV_32FC1);
		R[i] = cv::Scalar::all(R_lower);
		D[i].create(pFrame.size(), CV_32FC1);
		D[i] = cv::Scalar::all(0);
	}
	foregroundModel.create(pFrame.size(), CV_8UC1);
	foregroundModel = cv::Scalar::all(0);
	T.create(pFrame.size(), CV_32FC1);
	T = cv::Scalar::all(16.0);
	height = pFrame.rows;
	width = pFrame.cols;
	channels = pFrame.channels();
	stepUchar = pFrame.step[0];
	stepFloat = R[0].step[0];
	initBackgroundModel();
}

void CPBAS::initBackgroundModel()
{
	int row = 0, col = 0;
	for (int index = 0; index < N; ++index)
	{
		for (int i = 0; i < height; ++i)
		{
			float* p = backgroundModel[index].ptr<float>(i);
			for (int j = 0; j < width; ++j)
			{
				row = getX(i);
				col = getY(j);
				*p++ = static_cast<float>(*(pFrame.data + row * stepUchar + col));
			}
		}
	}
	Mat frameTmp;
	pFrame.convertTo(frameTmp, CV_32F);
	for (int i = 0; i < N; ++i)
	{
		absdiff(frameTmp, backgroundModel[i], D[i]);
	}
}

int CPBAS::getX(int x)
{
	x = x + c_yoff[random(0, patchsize)];
	if ( x < 0)
	{
		x = -x;
	}
	if (x >= height)
	{
		x = 2 * (height - 1) - height;
	}
	return x;
}

int CPBAS::getY(int y)
{
	y = y + c_xoff[random(0, patchsize)];
	if (y < 0)
	{
		y = -y;
	}
	if (y >= width)
	{
		y = 2 * (width - 1) - width;
	}
	return y;
}

int CPBAS::random(int a, int b)
{
	return rand() % (b - a) + a;
}

void CPBAS::getBackgroundModel(cv::Mat &image)
{
	image.create(pFrame.size(), CV_32FC1);
	image = Scalar::all(0);
	for (int i = 0; i < N; ++i)
	{

		image = image + backgroundModel[i];
	}
	image.convertTo(image, CV_8U, 1.0 / N, 0);
}

void CPBAS::operator()(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	img_input.copyTo(pFrame);
	if (pFrame.channels() == 3)
	{
		cvtColor(pFrame, pFrame, CV_BGR2GRAY);
	}
	GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
	if (firstTime)
	{
		init();
		firstTime = false;
	}
	int index;
	float sumD = 0;
	srand((int)time(NULL));
	for (int i = 0; i < height; ++i)
	{
		uchar* p = pFrame.ptr<uchar>(i);
		for (int k = 0; k < N; ++k)
		{
			pBGM[k] = backgroundModel[k].ptr<float>(i);
			pR[k] = R[k].ptr<float>(i);
			pD[k] = D[k].ptr<float>(i);
		}
		pT = T.ptr<float>(i);
		pFGM = foregroundModel.ptr<uchar>(i);
		for (int j = 0; j < width; ++j)
		{
			int count = 0;
			float dist = 0;
			sumD = 0;
			index = 0;
			int minindex = 0;
			float mindist = 256;
			while (index < N)
			{
				dist = std::abs(*(p + j) - *(pBGM[index] + j));
				if (dist < *(pR[index] + j))
				{
					++count;
				}
				if (mindist > dist)
				{
					mindist = dist;
					minindex = index;
				}
				sumD += dist;
				++index;
			}
			*(pD[minindex] + j) = mindist;
			//foreground judge
			//background
			if (count >= m_min)
			{
				*(pFGM + j) = 0;
				
				int rdm = random(0, static_cast<int>(*(pT + j)));
				if (rdm == 0)
				{
					rdm = random(0, N);
					*(pBGM[rdm] + j) = static_cast<float>(*(p + j));
				}
				//update neighbourhood
				rdm = random(0, static_cast<int>(*(pT + j)));
				if (rdm == 0)
				{
					int xng = i;
					int yng = j;
					while (i == xng && j == yng)
					{
						xng = getX(i);
						yng = getY(j);
					}
					rdm = random(0, static_cast<int>(*(pT + j)));
					float* tmp = backgroundModel[rdm].ptr<float>(xng);
					uchar* tmp1 = pFrame.ptr<uchar>(xng);
					*(tmp + yng) = static_cast<float>(*(tmp1 + yng));
				}
				//update T
				*(pT + j) = std::max(std::min(T_upper, *(pT + j) - T_dec / sumD * N), T_lower);
			}
			//foreground
			else
			{
				*(pFGM + j) = 255;
				//update T
				*(pT + j) = std::max(std::min(T_upper, *(pT + j) + T_inc / sumD * N), T_lower);
			}
			//upgrate R
			index = 0;
			while (index < N)
			{
				if (*(pR[index] + j) > (sumD * 1.0 / N * R_scale))
				{
					*(pR[index] + j) = std::max((*(pR[index] + j)) * (1 - R_inc_dec), R_lower);
				}
				else
				{
					*(pR[index] + j) = std::min((*(pR[index] + j)) * (1 + R_inc_dec), R_inc_dec);
				}
				++index;
			}
		}
	}
	foregroundModel.copyTo(img_output);
	getBackgroundModel(img_bgmodel);
}



