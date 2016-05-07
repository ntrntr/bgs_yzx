#pragma once
#include "IBGS.h"
#include "BackgroundSubtractorSuBSENSE.h"
class CSubsense :
	public IBGS
{
public:
	CSubsense();
	~CSubsense();
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
	void saveConfig();
	void loadConfig();
private:
	BackgroundSubtractorSuBSENSE* pLOBSTER;
	bool firstTime;
	bool showOutput;

	float fRelLBSPThreshold;
	size_t nLBSPThresholdOffset;
	size_t nDescDistThreshold;
	size_t nColorDistThreshold;
	size_t nBGSamples;
	size_t nRequiredBGSamples;
	cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
	//cv::Mat oSequenceROI;
};

