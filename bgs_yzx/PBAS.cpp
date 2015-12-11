#include "stdafx.h"
#include "PBAS.h"


CPBAS::CPBAS() : m_min(2), R_inc_dec(0.05), R_lower(18), R_scale(5), T_dec(0.05), T_inc(1.0), T_lower(2), T_upper(200), alpha(10), firstTime(true), patchsize(3)
{
	//backgroundModel.resize(N);
	//R.resize(N);
	//D.resize(N);  
	formerMeanMag = 20;
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
	
	//for (int i = 0; i < N; ++i)
	//{
		//backgroundModel[i].create(pFrame.size(), CV_32FC1);
		//backgroundModel[i] = cv::Scalar::all(0);
	backgroundModel.resize(pFrame.cols * pFrame.rows);
	backgroundGradient.resize(pFrame.cols * pFrame.rows);
		//R[i].create(pFrame.size(), CV_32FC1);
		//R[i] = cv::Scalar::all(R_lower);
		//D[i].create(pFrame.size(), CV_32FC1);
		//D[i] = cv::Scalar::all(0);
	//D.resize(pFrame.cols * pFrame.rows);
	//}
	R.create(pFrame.size(), CV_32FC1);
	R = cv::Scalar::all(R_lower);
	foregroundModel.create(pFrame.size(), CV_8UC1);
	foregroundModel = cv::Scalar::all(0);
	T.create(pFrame.size(), CV_32FC1);
	T = cv::Scalar::all(16.0);
	height = pFrame.rows;
	width = pFrame.cols;
	channels = pFrame.channels();
	//stepUchar = pFrame.step[0];
	//stepFloat = R[0].step[0];
	meanGradient.create(pFrame.size(), CV_32FC1);
	meanGradient = cv::Scalar::all(0.000001f);
	meanD.create(pFrame.size(), CV_32FC1);
	meanD = cv::Scalar::all(0.000001f);
}

void CPBAS::initBackgroundModel()
{
	//int row = 0, col = 0;
	//for (int index = 0; index < N; ++index)
	//{
	//	for (int i = 0; i < height; ++i)
	//	{
	//		float* p = backgroundModel[index].ptr<float>(i);
	//		for (int j = 0; j < width; ++j)
	//		{
	//			row = getX(i);
	//			col = getY(j);
	//			*p++ = static_cast<float>(*(pFrame.data + row * stepUchar + col));
	//		}
	//	}
	//}
	//int row = 0, col = 0;
	//for (int i = 0; i < height; ++i)
	//{
	//	for (int j = 0; j < width; ++j)
	//	{
	//		for (int k = 0; k < N; ++k)
	//		{
	//				row = getX(i);
	//				col = getY(j);
	//				backgroundModel[i * width + j].push_back(*(pFrame.data + row * pFrame.step[0] + col));
	//		}
	//		//backgroundModel[i * width + j].push_back(*p++);
	//	}
	//}
	//Mat frameTmp;
	//pFrame.convertTo(frameTmp, CV_32F);
	//for (int i = 0; i < N; ++i)
	//{
	//	absdiff(frameTmp, backgroundModel[i], D[i]);
	//}
	if (backgroundModel.front().size() < N)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{

				backgroundModel[i * width + j].push_back(*(pFrame.data + i * pFrame.step[0] + j));
			}
		}
	}
}

void CPBAS::initBackgroundGradient()
{
	if (backgroundGradient.front().size() < N)
	{
		Mat res;
		gradientComputation(res);
		for (int i = 0; i < height; ++i)
		{
			float* pRes = res.ptr<float>(i);
			for (int j = 0; j < width; ++j)
			{
				backgroundGradient[i * width + j].push_back(*(pRes + j));
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
	image.create(pFrame.size(), CV_8UC1);
	image = Scalar::all(0);
	int sum = 0;
	for (int i = 0; i < height; ++i)
	{
		uchar* pImage = image.ptr<uchar>(i);
		for (int j = 0; j < width; ++j)
		{
			sum = 0;
			for (int k = 0; k < backgroundModel[i * width + j].size(); ++k)
			{
				sum += backgroundModel[i * width + j][k];
			}
			*(pImage + j) = sum / backgroundModel[i * width + j].size();
		}
	}
	//for (int i = 0; i < N; ++i)
	//{

	//	image = image + backgroundModel[i];
	//}
	//image.convertTo(image, CV_8U, 1.0 / N, 0);
}

void CPBAS::gradientComputation(cv::Mat& res)
{
	Mat grad_x, grad_y;
	Sobel(pFrame, grad_x, CV_32F, 1, 0);
	Sobel(pFrame, grad_y, CV_32F, 0, 1);
	magnitude(grad_x, grad_y, res);
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
	initBackgroundModel();
	initBackgroundGradient();
	int index;
	float sumMag = 0;
	long int foregroundCount = 0;
	srand((int)time(NULL));
	gradientComputation(currentGradient);
	for (int i = 0; i < height; ++i)
	{
		uchar* p = pFrame.ptr<uchar>(i);
		pR = R.ptr<float>(i);
		pT = T.ptr<float>(i);
		pFGM = foregroundModel.ptr<uchar>(i);
		pMeanD = meanD.ptr<float>(i);
		for (int j = 0; j < width; ++j)
		{
			int count = 0;
			float dist = 0;
			float norm = 0;
			index = 0;
			int minindex = 0;
			float mindist = 1000;
			while (index < backgroundModel[i * width + j].size() && count < m_min)
			{
				dist = std::abs(*(p + j) - (backgroundModel[i * width + j][index]));
				norm = alpha / formerMeanMag * abs(currentGradient.at<float>(i, j) - backgroundGradient[i * width + j][index]);
				dist += norm;
				if (dist < *(pR + j))
				{
					++count;
				}
				else
				{
					sumMag += norm;
					++foregroundCount;
				}
				if (mindist > dist)
				{
					mindist = dist;
					minindex = index;
				}
				++index;
			}
			//*(pD[minindex] + j) = mindist;
			//foreground judge
			//background
			if (count >= m_min)
			{
				*(pFGM + j) = 0;
				//updatemean
				if (backgroundModel.front().size() <= N && backgroundModel.front().size() > 2)
				{
					*(pMeanD + j) = ((*(pMeanD + j)) * (backgroundModel.front().size() - 1) + mindist) / backgroundModel.front().size();
				}
				else if (backgroundModel.front().size() < N && backgroundModel.front().size() == 2)
				{
					*(pMeanD + j) = mindist;
				}
				if (backgroundModel.front().size() == N)
				{
					//update background model
					int rdm = random(0, static_cast<int>(*(pT + j)));
					if (rdm == 0)
					{
						rdm = random(0, N);
						backgroundModel[i * width + j][rdm] = (*(p + j));
						backgroundGradient[i * width + j][rdm] = currentGradient.at<float>(i, j);
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
						rdm = random(0, N);
						backgroundModel[xng * width + yng][rdm] = *(pFrame.data + xng * pFrame.step[0] + yng);
						backgroundGradient[i * width + j][rdm] = currentGradient.at<float>(xng, yng);
						//float* tmp = backgroundModel[rdm].ptr<float>(xng);
						//uchar* tmp1 = pFrame.ptr<uchar>(xng);
						//*(tmp + yng) = static_cast<float>(*(tmp1 + yng));
					}
					
				}
				//update T
				*(pT + j) = std::max(std::min(T_upper, *(pT + j) - T_dec / (*(pMeanD + j))), T_lower);
				
			}
			//foreground
			else
			{
				*(pFGM + j) = 255;
				//update T
				*(pT + j) = std::max(std::min(T_upper, *(pT + j) + T_inc / (*(pMeanD + j))), T_lower);
			}
			//upgrate R
			if (*(pR + j) > ((*(pMeanD + j)) * R_scale))
			{
				*(pR + j) = std::max((*(pR + j)) * (1 - R_inc_dec), R_lower);
			}
			else
			{
				*(pR + j) = std::min((*(pR + j)) * (1 + R_inc_dec), R_lower);
			}
			//while (index < N)
			//{
			//	if (*(pR[index] + j) > (sumD * 1.0 / N * R_scale))
			//	{
			//		*(pR[index] + j) = std::max((*(pR[index] + j)) * (1 - R_inc_dec), R_lower);
			//	}
			//	else
			//	{
			//		*(pR[index] + j) = std::min((*(pR[index] + j)) * (1 + R_inc_dec), R_inc_dec);
			//	}
			//	++index;
			//}
		}
	}
	//-> initiate some low value to prevent diving through zero
	double meanMag = sumMag  / (double)(foregroundCount + 1); 

	if (meanMag > 20)
		formerMeanMag = meanMag;
	else
		formerMeanMag = 20;
	foregroundModel.copyTo(img_output);
	getBackgroundModel(img_bgmodel);
}



