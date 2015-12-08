#include "stdafx.h"
#include "PBAS.h"


CPBAS::CPBAS() : m_min(2), R_inc_dec(0.05), R_lower(18), R_scale(5), T_dec(0.05), T_inc(1), T_lower(2), T_upper(200), alpha(10), firstTime(true), patchsize(3)
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
		D[i] = cv::Scalar::all(255);

	}
	foregroundModel.create(pFrame.size(), CV_8UC1);
	foregroundModel = cv::Scalar::all(0);
	T.create(pFrame.size(), CV_32FC1);
	T = cv::Scalar::all(16);
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
				*p++ = (*(pFrame.data + row * stepUchar + col));
			}
		}
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
	double mytmp;
	for (int i = 0; i < height; ++i)
	{
		uchar* p = pFrame.ptr<uchar>(i);
		for (int j = 0; j < width; ++j)
		{
			int count = 0;
			float dist = 0;
			sumD = 0;
			index = 0;
			while (index < N)
			{
				dist = abs(*p - (*(backgroundModel[index].data + i * stepFloat + j * channels)));
				mytmp = *(R[index].data + i * stepFloat + j * channels);
				if (dist < *(R[index].data + i * stepFloat + j * channels))
				{
					++count;
				}
				mytmp = *(D[index].data + i * stepFloat + j);
				*(D[index].data + i * stepFloat + j) = std::min(static_cast<float>(*(D[index].data + i * stepFloat + j)), dist);
				mytmp = *(D[index].data + i * stepFloat + j);
				sumD += *(D[index].data + i * stepFloat + j);
				++index;
			}
			//upgrate R
			index = 0;
			while (index < N)
			{
				if (*(R[index].data + i * stepFloat + j) >(sumD *1.0 / N * R_scale))
				{
					*(R[index].data + i * stepFloat + j) = std::min((*(R[index].data + i * stepFloat + j)) * (1 - R_inc_dec), R_lower);
				}
				else
				{
					*(R[index].data + i * stepFloat + j) = (*(R[index].data + i * stepFloat + j)) * (1 + R_inc_dec);
				}
				++index;
			}
			

			//foreground judge
			//background
			if (count >= m_min)
			{
				*(foregroundModel.data + i * stepUchar + j) = 0;
				int rdm = random(0, static_cast<int>(*(T.data + i * stepFloat + j)));
				if (rdm == 0)
				{
					rdm = random(0, N);
					*(backgroundModel[rdm].data + i * stepFloat + j) = static_cast<float>(*p);
				}
				//update neighbourhood
				rdm = random(0, static_cast<int>(*(T.data + i * stepFloat + j)));
				if (rdm == 0)
				{
					int xng = 0;
					int yng = 0;
					while ((xng == 0 && yng == 0) || ((i + xng) < 0 || (i + xng) >= height) || ((j + yng) < 0 || (j + yng) >= width))
					{
						xng = random(-patchsize / 2, patchsize / 2 + 1); //[-1,1]
						yng = random(-patchsize / 2, patchsize / 2 + 1); //[-1,1]
					}
					int myx = i + xng;
					int myy = j + yng;
					rdm = random(0, static_cast<int>(*(T.data + i * stepFloat + j)));
					*(backgroundModel[rdm].data + myx * stepFloat + myy) = static_cast<float>(*p);
				}
				//update T
				*(T.data + i * stepFloat + j) = min(max(*(T.data + i * stepFloat + j) - T_dec / (sumD / N), T_upper), T_lower);
			}
			//foreground
			else
			{
				*(foregroundModel.data + i * stepUchar + j) = 255;
				//update T
				*(T.data + i * stepFloat + j) = min(max(*(T.data + i * stepFloat + j) + T_inc / (sumD / N), T_upper), T_lower);
			}
		}
	}
	foregroundModel.copyTo(img_output);
	getBackgroundModel(img_bgmodel);
}



