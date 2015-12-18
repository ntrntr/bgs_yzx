#include "stdafx.h"
#include "Codebook2BGS.h"


CCodebook2BGS::CCodebook2BGS() :firstTime(true), nframes(1), model(NULL), NCHANNELS(3), nframesToLearnBG(100)
{
	for (int i = 0; i < 3; ++i)
	{
		ch[i] = true;
	}
}


CCodebook2BGS::~CCodebook2BGS()
{
	if (model)
	{
		//delete[] model;
	}
}

void CCodebook2BGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
	{
		return;
	}
	loadConfig();
	if (firstTime)
	{

		saveConfig();
		initMemberVariable(img_input);
		firstTime = false;
	}
	if (!img_input.empty())
	{
		cvtColor(img_input, yuvImage, CV_BGR2YCrCb);
		if (nframes-1 < nframesToLearnBG)
		{
			cvBGCodeBookUpdate(model, &yuvImage.operator IplImage());
		}
		if (nframes-1 == nframesToLearnBG)
		{
			cvBGCodeBookClearStale(model, model->t / 2);

		}
		if (nframes - 1 >= nframesToLearnBG)
		{
			cvBGCodeBookDiff(model, &yuvImage.operator IplImage(), &ImaskCodeBook.operator IplImage());
			ImaskCodeBook.copyTo(ImaskCodeBookCC);
			cvSegmentFGMask(&ImaskCodeBookCC.operator IplImage());
		}
	}
	ImaskCodeBook.copyTo(img_output);
	++nframes;
}

void CCodebook2BGS::initMemberVariable(const cv::Mat &img_input)
{
	model = cvCreateBGCodeBookModel();
	//Set color thresholds to default values
	model->modMin[0] = 3;
	model->modMin[1] = model->modMin[2] = 3;
	model->modMax[0] = 10;
	model->modMax[1] = model->modMax[2] = 10;
	model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;
	img_input.copyTo(yuvImage);
	ImaskCodeBook.create(img_input.size(), CV_8UC1);
	ImaskCodeBook = Scalar::all(0);
	ImaskCodeBookCC.create(img_input.size(), CV_8UC1);

}

void CCodebook2BGS::saveConfig()
{

}

void CCodebook2BGS::loadConfig()
{

}
