#include "stdafx.h"
#include "ImproveVibeTest.h"


CImproveVibeTest::CImproveVibeTest() :alpha(0), N(MODEL_SIZE), R(20), _min(2), ts(16), patchsize(3), firstFrame(true), needRebuiltModel(true), fgcount(0), updateFactor(5), frameCount(1)
{
	for (int i = 0; i < 256; ++i)
	{
		AlphaLUT[i] = std::pow(alpha, i);
	}

}


CImproveVibeTest::~CImproveVibeTest()
{

}

int CImproveVibeTest::random(int a, int b)
{
	return rand() % (b - a) + a;
	//return rng.uniform(a, b);
}

void CImproveVibeTest::modifyMask(cv::Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 5);
}

void CImproveVibeTest::initBackgroundModel()
{

	int x, y;
	//every pixel have 20 samples,not random
	for (int index = 0; index < N; ++index)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				x = getRandomX(i);
				y = getRandomY(j);
				for (int k = 0; k < 3; ++k)
				{
					simdata[index][i * step + j * pFrame.step[1] + k] = data[x * step + y * pFrame.step[1] + k];
				}

			}
		}
	}
	needRebuiltModel = false;
}

void CImproveVibeTest::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate)
{

	image.copyTo(pFrame);
	GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
	cvtColor(pFrame, pFrame, CV_BGR2YCrCb);
	fgmask.create(pFrame.size(), CV_8UC1);
	sg = (uchar*)fgmask.data;
	data = pFrame.data;
	srand((int)time(NULL));
	//gradientComputation(pFrame);
	if (firstFrame)
	{
		for (int index = 0; index < N; ++index)
		{
			simple[index].create(pFrame.size(), CV_8UC3);
			simdata[index] = (uchar*)simple[index].data;
		}
		height = pFrame.rows;
		width = pFrame.cols;
		//step is the bytes of each line
		step = pFrame.step[0];
		//chanels ,for rgb is 3
		chanels = pFrame.channels();
		//light threshold, 55% of the image change to foregrond, means have a light change
		beta = 0.55 * height * width;
		firstFrame = false;
		oR.create(pFrame.size(), CV_32FC1);
		oR = Scalar::all(R);
		cvtColor(pFrame, prevFrameGray, CV_BGR2GRAY);
		diffData.resize(pFrame.rows * pFrame.cols);
	}
	if (frameCount <= learningRate)
	{
		cvtColor(pFrame, curFrameGray, CV_BGR2GRAY);
		absdiff(prevFrameGray, curFrameGray, diffMap);
		for (int i = 0; i < diffMap.rows; ++i)
		{
			uchar* p = curFrameGray.ptr<uchar>(i);
			for (int j = 0; j < diffMap.cols; ++j, ++p)
			{
				diffData[i * diffMap.rows + j].push_back((float)(*p));
			}
		}
		if (frameCount == learningRate)
		{
			ofstream outfile;
			outfile.open("Vibe_diff.txt");
			Scalar_<double> meanData, dev;
			for (int i = 0; i < diffMap.rows; ++i)
			{
				float* p = oR.ptr<float>(i);
				for (int j = 0; j < diffMap.cols; ++j, ++p)
				{
					meanStdDev(diffData[i * diffMap.rows + j], meanData, dev);
					(*p) = std::min((float)(255.0),std::max(*p, (float)(dev[0])));
					outfile << "(" << i << "," << j << ") " << meanData[0] << " " << dev[0] << "|";
				}
				outfile << endl;
			}

			//imshow("test", oR);
			
			outfile << oR << endl;;
			//for (int i = 0; i < diffData.size()/10; ++i)
			//{
			//	//outfile << "point : x = " << watchPoint[i].x << ", y = " << watchPoint[i].y << endl;
			//	cout << "point index = " << i << endl;
			//	for (auto j : diffData[i])
			//	{
			//		

			//		outfile << j << " ";
			//		
			//	}
			//	outfile << endl;
			//}
			outfile.close();
		}
	}
	if (needRebuiltModel)
	{
		initBackgroundModel();
	}
	else
	{

		int x, y, index;
		double Rdeviation;
		//#pragma omp parallel for reduction(+:fgcount)
		for (x = 0; x < width; ++x)
		{
			double a[3] = { 0, 0, 0 };
			for (y = 0; y < height; ++y)
			{
				//y height, x width 
				//compare pixel to background model
				int count = 0;
				double dist = 0;
				index = 0;
				//Rdeviation = getStandardDeviationR(x, y);
				while (count < _min && index < N)
				{
					////euclidean distance computation
					a[0] = data[y * step + x * pFrame.step[1] + 0] - simdata[index][y * step + x * pFrame.step[1] + 0];
					a[1] = data[y * step + x * pFrame.step[1] + 1] - simdata[index][y * step + x * pFrame.step[1] + 1];
					a[2] = data[y * step + x * pFrame.step[1] + 2] - simdata[index][y * step + x * pFrame.step[1] + 2];

					dist = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
					//dist = getColordist(&data[y * step + x * pFrame.step[1]], &simdata[index][y * step + x * pFrame.step[1]]);

					if (dist < oR.at<float>(y, x))
					{
						++count;
					}
					++index;
				}
				//classify pixel and update model
				if (count >= _min)
				{
					//store that image[x][y] to background
					sg[y * fgmask.step[0] + x * fgmask.step[1]] = 0;
					//1 / ts to change
					int rdm = rng.uniform(0, ts);
					if (rdm == 0)
					{
						for (int k = 0; k < updateFactor; ++k)
						{
							rdm = rng.uniform(0, N);
							simdata[rdm][y * step + x * pFrame.step[1] + 0] = data[y * step + x * pFrame.step[1] + 0];
							simdata[rdm][y * step + x * pFrame.step[1] + 1] = data[y * step + x * pFrame.step[1] + 1];
							simdata[rdm][y * step + x * pFrame.step[1] + 2] = data[y * step + x * pFrame.step[1] + 2];
						}

					}
					//update neighboring pixel model
					rdm = rng.uniform(0, ts);
					if (rdm == 0 /*&& gradient.at<uchar>(y, x) <= 50*/)
					{
						for (int k = 0; k < updateFactor; ++k)
						{
							int xng = getRandomY(x);
							int yng = getRandomX(y);
							rdm = rng.uniform(0, N);
							//omp_set_lock(&mylock);
							//x¡Ê[1,width -2],y¡Ê[1,height -2],so will not out of range
							simdata[rdm][yng * step + xng * pFrame.step[1] + 0] = data[y * step + x * pFrame.step[1] + 0];
							simdata[rdm][yng * step + xng * pFrame.step[1] + 1] = data[y * step + x * pFrame.step[1] + 1];
							simdata[rdm][yng * step + xng * pFrame.step[1] + 2] = data[y * step + x * pFrame.step[1] + 2];
							//omp_unset_lock(&mylock);
						}

					}

				}
				else
				{
					// count < min
					//store that image[x][y] = foreground;
					sg[y * fgmask.step[0] + x * fgmask.step[1]] = 255;
					++fgcount;
				}
			}
		}
		if (fgcount > beta)
		{
			needRebuiltModel = true;
		}
		//next frame
		fgcount = 0;

	}
	++frameCount;
	//removeSmallBlobs(fgmask, 1000);
}

void CImproveVibeTest::saveBackgroundModels(cv::Mat& image)
{

	image.create(pFrame.size(), CV_32FC3);
	image = Scalar::all(0);
	Mat tmp;
	for (int i = 0; i < N; ++i)
	{
		simple[i].convertTo(tmp, CV_32F, 1.0, 0.0);
		image = image + tmp;
	}
	image.convertTo(image, CV_8U, 1.0 / N, 0);
	cvtColor(image, image, CV_YCrCb2BGR);
}

int CImproveVibeTest::getRandomX(int x)
{
	x = x + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
	if (x < 0)
	{
		x = -x;
	}
	else if (x >= height)
	{
		x = 2 * (height - 1) - x;
	}
	return x;

}

int CImproveVibeTest::getRandomY(int y)
{
	y = y + rng.uniform((int)(-patchsize / 2), (int)(patchsize / 2 + 1));
	if (y < 0)
	{
		y = -y;
	}
	if (y >= width)
	{
		y = 2 * (width - 1) - y;
	}
	return y;
}

void CImproveVibeTest::removeSmallBlobs(cv::Mat& im, double size)
{
	if (im.channels() != 1 || im.type() != CV_8U)
	{
		return;
	}
	vector<vector<Point>> contours;
	findContours(im, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i)
	{
		double area = cv::contourArea(contours[i]);
		if (area > 0 && area <= size)
		{
			cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
		}
	}
}

void CImproveVibeTest::gradientComputation(cv::Mat& res)
{

	Mat grad_x, grad_y;
	Mat tmp;
	if (res.channels() == 3)
	{
		cvtColor(res, tmp, CV_BGR2GRAY);
	}

	Sobel(tmp, grad_x, CV_32F, 1, 0);
	Sobel(tmp, grad_y, CV_32F, 0, 1);
	magnitude(grad_x, grad_y, gradient);
	//double minNum, maxNum;
	/*minMaxLoc(res, &minNum, &minNum);
	res.convertTo(res, CV_8U, )*/
	cv::normalize(gradient, gradient, 0, 255, NORM_MINMAX, CV_8UC1);
}

double CImproveVibeTest::getColordist(uchar* p, uchar* q)
{
	int xt = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
	int vt = q[0] * q[0] + q[1] * q[1] + q[2] * q[2];
	int p2 = p[0] * q[0] + p[1] * q[1] + p[2] * q[2];
	return static_cast<int>(sqrt(xt - 1.0 * p2 * p2 / vt));
}

double CImproveVibeTest::getStandardDeviationR(int x, int y)
{
	int mean = 0;
	double res = 0;
	for (int i = 0; i < N; ++i)
	{
		mean += simdata[i][y * step + x * pFrame.step[1] + 0] + simdata[i][y * step + x * pFrame.step[1] + 1] + simdata[i][y * step + x * pFrame.step[1] + 2];
	}
	mean = mean / N;
	for (int i = 0; i < N; ++i)
	{
		res += (simdata[i][y * step + x * pFrame.step[1] + 0] + simdata[i][y * step + x * pFrame.step[1] + 1] + simdata[i][y * step + x * pFrame.step[1] + 2] - mean) * (simdata[i][y * step + x * pFrame.step[1] + 0] + simdata[i][y * step + x * pFrame.step[1] + 1] + simdata[i][y * step + x * pFrame.step[1] + 2] - mean);
	}
	res /= N;
	res *= 0.5;
	res += 20;
	if (res < 20)
	{
		res = 20;
	}
	else if (res > 40)
	{
		res = 40;
	}
	return res;
}
