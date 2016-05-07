#include "stdafx.h"
#include "BGSubtract.h"


CBGSubtract::CBGSubtract() :nframes(0), frameSize(Size(0, 0)), frameType(0), history(500), alpha(0.5), firstTime(true)
{
	std::cout << "CBGSubtract()" << std::endl;
}


CBGSubtract::~CBGSubtract()
{
	std::cout << "~CBGSubtract()" << std::endl;
}

void CBGSubtract::operator()(cv::InputArray _image, cv::OutputArray _fgmask)
{
	Mat image = _image.getMat();
	cv::cvtColor(image, image, CV_BGR2GRAY);
	//GaussianBlur(image, image, Size(3, 3), 0);
	image.convertTo(image, CV_32F);
	bool needToInitialize = nframes == 0 || image.size() != frameSize || image.type() != frameType;
	if (needToInitialize)
	{
		initialize(image.size(), image.type());
		image.copyTo(backgroundModel);
		backgroundModel.convertTo(backgroundModel, CV_32F);
		needToInitialize = false;
	}

	cv::absdiff(image, backgroundModel, img_foreground);
	//if (img_foreground.channels() == 3)
	//{
	//	cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);
	//}
	cv::threshold(img_foreground, img_foreground, 20.0, 255, cv::THRESH_BINARY);
	img_foreground.convertTo(img_foreground, CV_8U);
	img_foreground.copyTo(_fgmask);
	++nframes;
	updateBackgroundModel(img_foreground, image);
}

void CBGSubtract::initialize(cv::Size _frameSize, int _frameType)
{
	frameSize = _frameSize;
	frameType = _frameType;

	int nchannels = CV_MAT_CN(frameType);
}

void CBGSubtract::saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count)
{
	char str[30];
	sprintf_s(str, filename.c_str(), count);
	string tmp = outpath + str;
	cv::imwrite(tmp.c_str(), image);
}

void CBGSubtract::modifyMask(cv::Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 3);
}

void CBGSubtract::updateBackgroundModel(const cv::Mat& mask, const cv::Mat& input)
{
	float* data;
	const uchar* pmask;
	const float* pinput;
	int nchannels = backgroundModel.channels();
	for (int y = 0; y < backgroundModel.rows; ++y)
	{

		data = backgroundModel.ptr<float>(y);
		pmask = mask.ptr<uchar>(y);
		pinput = input.ptr<float>(y);
		for (int x = 0; x < backgroundModel.cols; ++x, data += nchannels, pinput += nchannels, pmask++)
		{
			if (*pmask > 0)
			{
				continue;
			}
			else
			{
				for (int c = 0; c < nchannels; ++c)
				{
					data[c] = (float)((1.0 - pow(alpha, 1.0 * abs(data[c] - pinput[c]))) * data[c] + 1.0 * pow(alpha, abs(data[c] - pinput[c])) * pinput[c]);
				}
			}
		}
	}
}

void CBGSubtract::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{
		saveConfig();
		firstTime = false;
	}
	operator()(img_input, img_output);
	backgroundModel.copyTo(img_bgmodel);
	img_bgmodel.convertTo(img_bgmodel, CV_8U);
}

void CBGSubtract::saveConfig()
{

}

void CBGSubtract::loadConfig()
{

}
