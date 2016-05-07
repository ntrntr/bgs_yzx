#pragma once
#include "IBGS.h"
class CBGSubtract :
	public IBGS
{
public:
	CBGSubtract();
	~CBGSubtract();
	void operator()(cv::InputArray _image, cv::OutputArray _fgmask);
	void initialize(cv::Size _frameSize, int _frameType);
	void saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count);
	void modifyMask(cv::Mat& mask);
	void updateBackgroundModel(const cv::Mat& mask, const cv::Mat& input);
public:
	cv::Mat img_foreground;
	cv::Size frameSize;
	int frameType;
	cv::Mat backgroundModel;
	int nframes;
	int history;
	float alpha;
	bool firstTime;
public:
	void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
private:
	void saveConfig();
	void loadConfig();
};

