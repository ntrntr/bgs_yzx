#include "stdafx.h"
#include "AdpativeyzxVibe.h"


CAdpativeyzxVibe::CAdpativeyzxVibe() :alpha(0.1), N(MODEL_SIZE2), R(20), _min(2), ts(16), patchsize(3), firstFrame(true), needRebuiltModel(true), fgcount(0)
{
	for (int i = 0; i < 256; ++i)
	{
		AlphaLUT[i] = std::pow(alpha, i);
	}
	c_xoff.resize(patchsize);
	c_yoff.resize(patchsize);
	for (int tmp = -patchsize / 2, i = 0; i < patchsize; ++i, ++tmp)
	{
		c_xoff[i] = tmp;
		c_yoff[i] = tmp;
	}
}


CAdpativeyzxVibe::~CAdpativeyzxVibe()
{

}

int CAdpativeyzxVibe::random(int a, int b)
{
	return rand() % (b - a) + a;
}

void CAdpativeyzxVibe::modifyMask(cv::Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 5);
}

void CAdpativeyzxVibe::initBackgroundModel()
{

	int x, y;
	//every pixel have 20 samples,not random
	for (int index = 0; index < N; ++index)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				int tmp = random(0, patchsize);
				x = i + c_yoff[tmp];
				//cout<<"tmp = "<<tmp <<"\t"<<"c_yoff[tmp] = "<<c_yoff[tmp]<<endl;
				if (x < 0)
				{
					x = -x;
				}
				if (x >= height)
				{
					x = 2 * (height - 1) - height;
				}
				tmp = random(0, patchsize);
				y = j + c_xoff[tmp];
				//cout<<"tmp = "<<tmp <<"\t"<<"c_xoff[tmp] = "<<c_xoff[tmp]<<endl;
				if (y < 0)
				{
					y = -y;
				}
				if (y >= width)
				{
					y = 2 * (width - 1) - width;
				}
				for (int k = 0; k < 3; ++k)
				{
					//data = image data;
					simdata[index][i * step + j * pFrame.step[1] + k] = data[x * step + y * pFrame.step[1] + k];
				}

			}
		}
	}
	needRebuiltModel = false;
}

void CAdpativeyzxVibe::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate)
{

	image.copyTo(pFrame);

	GaussianBlur(pFrame, pFrame, Size(3, 3), 0);
	fgmask.create(pFrame.size(), CV_8UC1);
	sg = (uchar*)fgmask.data;
	data = pFrame.data;
	if (firstFrame)
	{
		for (int index = 0; index < N; ++index)
		{
			/*simple[index] = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 3);*/
			simple[index].create(pFrame.size(), CV_8UC3);
			//simdata[index] = (uchar*)simple[index]->imageData;
			simdata[index] = (uchar*)simple[index].data;

		}

		//segmap = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

		height = pFrame.rows;
		width = pFrame.cols;
		//step is the bytes of each line
		step = pFrame.step[0];
		//chanels ,for rgb is 3
		chanels = pFrame.channels();
		//light threshold, 55% of the image change to foregrond, means have a light change
		beta = 0.55 * height * width;
		firstFrame = false;
	}

	if (needRebuiltModel)
	{
		initBackgroundModel();
	}
	else
	{

		int x, y, index;
		//#pragma omp parallel for reduction(+:fgcount)
		for (x = 1; x < width - 1; ++x)
		{
			double a[3] = { 0, 0, 0 };
			for (y = 1; y < height - 1; ++y)
			{
				//y height, x width 
				//compare pixel to background model
				int count = 0;
				double dist = 0;
				index = 0;
				while (count < _min && index < N)
				{
					//euclidean distance computation
					a[0] = data[y * step + x * pFrame.step[1] + 0] - simdata[index][y * step + x * pFrame.step[1] + 0];
					a[1] = data[y * step + x * pFrame.step[1] + 1] - simdata[index][y * step + x * pFrame.step[1] + 1];
					a[2] = data[y * step + x * pFrame.step[1] + 2] - simdata[index][y * step + x * pFrame.step[1] + 2];

					dist = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
					if (dist < R)
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
					int rdm = random(0, ts);
					if (rdm == 0)
					{
						rdm = random(0, N);
						//omp_set_lock(&mylock);
						//simdata[rdm][y * step + x * pFrame.step[1] + 0] = data[y * step + x * pFrame.step[1] + 0];
						//simdata[rdm][y * step + x * pFrame.step[1] + 1] = data[y * step + x * pFrame.step[1] + 1];
						//simdata[rdm][y * step + x * pFrame.step[1] + 2] = data[y * step + x * pFrame.step[1] + 2];

						
						simdata[rdm][y * step + x * pFrame.step[1] + 0] = (1 - AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 0] - data[y * step + x * pFrame.step[1] + 0])]) * simdata[rdm][y * step + x * pFrame.step[1] + 0] + AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 0] - data[y * step + x * pFrame.step[1] + 0])] * data[y * step + x * pFrame.step[1] + 0];

						simdata[rdm][y * step + x * pFrame.step[1] + 1] = (1 - AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 1] - data[y * step + x * pFrame.step[1] + 1])]) * simdata[rdm][y * step + x * pFrame.step[1] + 1] + AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 1] - data[y * step + x * pFrame.step[1] + 1])] * data[y * step + x * pFrame.step[1] + 1];

						simdata[rdm][y * step + x * pFrame.step[1] + 2] = (1 - AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 2] - data[y * step + x * pFrame.step[1] + 2])]) * simdata[rdm][y * step + x * pFrame.step[1] + 2] + AlphaLUT[(int)abs(simdata[rdm][y * step + x * pFrame.step[1] + 2] - data[y * step + x * pFrame.step[1] + 2])] * data[y * step + x * pFrame.step[1] + 2];
						
						//omp_unset_lock(&mylock);
					}
					//update neighboring pixel model
					rdm = random(0, ts);
					if (rdm == 0)
					{
						int xng = 0;
						int yng = 0;
						while (xng == 0 && yng == 0)
						{
							xng = random(-1, 2); //[-1,1]
							yng = random(-1, 2); //[-1,1]
						}
						rdm = random(0, ts);
						//omp_set_lock(&mylock);
						//x¡Ê[1,width -2],y¡Ê[1,height -2],so will not out of range
						simdata[rdm][(y + yng) * step + (x + xng) * pFrame.step[1] + 0] = data[y * step + x * pFrame.step[1] + 0];
						simdata[rdm][(y + yng) * step + (x + xng) * pFrame.step[1] + 1] = data[y * step + x * pFrame.step[1] + 1];
						simdata[rdm][(y + yng) * step + (x + xng) * pFrame.step[1] + 2] = data[y * step + x * pFrame.step[1] + 2];
						//omp_unset_lock(&mylock);
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

}

void CAdpativeyzxVibe::saveBackgroundModels(cv::Mat& image)
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
}
