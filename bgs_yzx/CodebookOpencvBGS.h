#pragma once
#include "cv_yuv_codebook.h"
#include "IBGS.h"

class CCodebookOpencvBGS :
	public IBGS
{
public:
	CCodebookOpencvBGS();
	~CCodebookOpencvBGS();
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
	Mat rawImage;
	Mat yuvImage;
	Mat ImaskCodeBook;
	codeBook* cB;
	unsigned cbBounds[CHANNELS];
	uchar* pColor;
	int imageLen;
	int nChannels;
	int minMod[CHANNELS];
	int maxMod[CHANNELS];
};

