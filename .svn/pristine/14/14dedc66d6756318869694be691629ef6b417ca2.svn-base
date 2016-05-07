#pragma once
#include "IBGS.h"
class CCodebook2BGS :
	public IBGS
{
public:
	CCodebook2BGS();
	~CCodebook2BGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void initMemberVariable(const cv::Mat &img_input);
private:
	void saveConfig();
	void loadConfig();
private:
	bool firstTime;
	int nframes;
	int learnLength;
	CvBGCodeBookModel* model;
	int NCHANNELS;
	bool ch[3];
	int nframesToLearnBG;
	Mat rawImage;
	Mat yuvImage;
	Mat ImaskCodeBook;
	Mat ImaskCodeBookCC;
};

