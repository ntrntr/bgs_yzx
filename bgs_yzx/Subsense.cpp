#include "stdafx.h"
#include "Subsense.h"


CSubsense::CSubsense():
pLOBSTER(0), firstTime(true), showOutput(true),
fRelLBSPThreshold(BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
nLBSPThresholdOffset(BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
nDescDistThreshold(BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
nColorDistThreshold(BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
nBGSamples(BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES),
nRequiredBGSamples(BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES)
{
}


CSubsense::~CSubsense()
{
	if (pLOBSTER)
	{
		delete pLOBSTER;
	}
}

void CSubsense::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

	loadConfig();

	if (firstTime) {
		saveConfig();
		oCurrSegmMask.create(img_input.size(), CV_8UC1);
		oCurrReconstrBGImg.create(oCurrInputFrame.size(), oCurrInputFrame.type());
		//oSequenceROI.create(img_input.size(), CV_8UC1);
		//oSequenceROI = cv::Scalar(255);
		pLOBSTER = new BackgroundSubtractorSuBSENSE;

		pLOBSTER->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
		firstTime = false;
	}

	(*pLOBSTER)(img_input, img_output,double(100));
	pLOBSTER->getBackgroundImage(img_bgmodel);

	if (showOutput) {
		imshow("LOBSTER FG", img_output);
		imshow("LOBSTER BG", img_bgmodel);
	}
}

void CSubsense::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/LOBSTERBGS.xml", 0, CV_STORAGE_WRITE);

	cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
	cvWriteInt(fs, "nLBSPThresholdOffset", nLBSPThresholdOffset);
	cvWriteInt(fs, "nDescDistThreshold", nDescDistThreshold);
	cvWriteInt(fs, "nColorDistThreshold", nColorDistThreshold);
	cvWriteInt(fs, "nBGSamples", nBGSamples);
	cvWriteInt(fs, "nRequiredBGSamples", nRequiredBGSamples);
	cvWriteInt(fs, "showOutput", showOutput);

	cvReleaseFileStorage(&fs);
}

void CSubsense::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/LOBSTERBGS.xml", 0, CV_STORAGE_READ);

	fRelLBSPThreshold = cvReadRealByName(fs, 0, "fRelLBSPThreshold", BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
	nLBSPThresholdOffset = cvReadIntByName(fs, 0, "nLBSPThresholdOffset", BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET);
	nDescDistThreshold = cvReadIntByName(fs, 0, "nDescDistThreshold", BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET);
	nColorDistThreshold = cvReadIntByName(fs, 0, "nColorDistThreshold", BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD);
	nBGSamples = cvReadIntByName(fs, 0, "nBGSamples", BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES);
	nRequiredBGSamples = cvReadIntByName(fs, 0, "nRequiredBGSamples", BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES);
	showOutput = cvReadIntByName(fs, 0, "showOutput", true);

	cvReleaseFileStorage(&fs);
}
