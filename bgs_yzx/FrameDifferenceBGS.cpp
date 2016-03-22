#include "stdafx.h"
#include "FrameDifferenceBGS.h"
#include "commom.h"

FrameDifferenceBGS::FrameDifferenceBGS() : firstTime(true), enableThreshold(true), threshold(15), showOutput(true)
{
	std::cout << "FrameDifferenceBGS()" << std::endl;
}


FrameDifferenceBGS::~FrameDifferenceBGS()
{
	std::cout << "~FrameDifferenceBGS()" << std::endl;
}

void FrameDifferenceBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime)
		saveConfig();

	if (img_input_prev.empty())
	{
		img_input.copyTo(img_input_prev);
		return;
	}

	cv::absdiff(img_input_prev, img_input, img_foreground);

	if (img_foreground.channels() == 3)
		cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

	if (enableThreshold)
		cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

	if (showOutput)
		cv::imshow("Frame Difference", img_foreground);

	img_foreground.copyTo(img_output);
	img_input_prev.copyTo(img_bgmodel);
	img_input.copyTo(img_input_prev);

	firstTime = false;
}

void FrameDifferenceBGS::saveConfig()
{
	//CvFileStorage* fs = cvOpenFileStorage("./config/FrameDifferenceBGS.xml", 0, CV_STORAGE_WRITE);
	//cvWriteString(fs, "calibrationData", getCurrentTime().c_str());
	//cvWriteInt(fs, "enableThreshold", enableThreshold);
	//cvWriteInt(fs, "threshold", threshold);
	//cvWriteInt(fs, "showOutput", showOutput);
	//cvReleaseFileStorage(&fs);
	//cv::FileStorage fs("./config/FrameDifferenceBGS.xml", cv::FileStorage::WRITE);
	//fs << "calibrationData" << buf;
	//fs << "enableThreshold" << enableThreshold;
	//fs << "threshold" << threshold;
	//fs << "showOutput" << showOutput;
	//fs.release();
}

void FrameDifferenceBGS::loadConfig()
{
	enableThreshold = true;
	threshold = 15;
	showOutput = false;
	//CvFileStorage* fs = cvOpenFileStorage("./config/FrameDifferenceBGS.xml", 0, CV_STORAGE_READ);

	//enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
	//threshold = cvReadIntByName(fs, 0, "threshold", 15);
	//showOutput = cvReadIntByName(fs, 0, "showOutput", true);

	//cvReleaseFileStorage(&fs);
	//FileStorage fs;
	//cv::FileStorage fs("./config/FrameDifferenceBGS.xml", cv::FileStorage::READ);
	//fs["enableThreshold"] >> enableThreshold;
	//fs["threshold"] >> threshold;
	//fs["showOutput"] >> showOutput;
	//fs.release();
}
