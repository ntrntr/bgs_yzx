#pragma once
class CMultiscaleSTBG
{
public:
	CMultiscaleSTBG();
	~CMultiscaleSTBG();
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask);
	void getBackgroundModel(cv::Mat& image);
	void init();
	vector<Mat> getPyr(const Mat& src, int level = 3);
	void initBGModel();
	void initLevelBGModel(int level, const Mat& src, Size size);
	Point getRandomXY(Point pos, Size size);
	double getPointProbability(int x, int y, const Mat& frame, const vector<Mat>& bgmodel);
	void updateBGModel(int level);
	void updateLevelBGModel(int x, int y, double proba, int level);
	void getforeMask(Mat& img);
private:
	bool firstTime;
	int N;
	double T1;
	double T2;
	int level;
	double Rthreshold;
	Mat foreground;
	vector<vector<Mat>> BGModel;
	Mat pFrame;
	vector<Mat> pFramePyr;
	Mat probability;
	Mat probleMap;
	RNG rng;
};

