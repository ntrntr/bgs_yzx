#pragma once
#include "IBGS.h"
#include "YzxCodeBook.h"



class CYZXCodebookBGS :
	public IBGS
{
public:
	CYZXCodebookBGS();
	~CYZXCodebookBGS();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
	void initMemberVariable(const cv::Mat &img_input);
private:
	void saveConfig();
	void loadConfig();
private:
	bool firstTime;
	YzxCodeBook gbs;
	const int learnLength;
	int nframes;
private:
	Mat yuvImage;
	Mat maskCodeBook;
	codeBook* cb;
	unsigned cbBounds[CHANNELS];
	uchar* pRGB;
	uchar* pColor;
	int imageLen;
	int nChannels = CHANNELS;
	int minMod[CHANNELS];
	int maxMod[CHANNELS];
};

