#pragma once
class CImproveVibeTest
{
public:
	CImproveVibeTest();
	~CImproveVibeTest();

public:
	int random(int a, int b);
	void modifyMask(cv::Mat& mask);
	void initBackgroundModel();
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 200);
	void saveBackgroundModels(cv::Mat& image);
	int getRandomX(int x);
	int getRandomY(int y);
	void removeSmallBlobs(cv::Mat& im, double size);
	void gradientComputation(cv::Mat& res);
	double getColordist(uchar* p, uchar* q);
	double getStandardDeviationR(int x, int y);
	double getMean(vector<int>& data);
	double getStd(vector<int>& data, double mean);
private:
	double alpha;
	Mat pFrame;
	Mat simple[MODEL_SIZE];
	map<int, double> AlphaLUT;
	Mat gradient;
	const int N;
	const double R;
	const int _min;
	const int ts;
	const int patchsize;
	bool firstFrame;
	int height, width, step, chanels;
	double beta;
	bool needRebuiltModel;
	int fgcount;
	uchar* data;
	uchar* simdata[MODEL_SIZE];
	uchar* sg;
	cv::RNG rng;
	int updateFactor;
	Mat oR;
	Mat prevFrameGray;
	Mat curFrameGray;
	Mat diffMap;
	int frameCount;
	vector<vector<int>> diffData;
	Mat outDiffMap;
	long int mycount;
};

