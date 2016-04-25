#pragma once
struct ltps_node
{
	double weight;
	double x;
	double y;
	bool operator< (const ltps_node& b)
	{
		return weight > b.weight;
	}
	ltps_node(){};
	ltps_node(double a, double b, double w = 0.3) :x(a), y(b), weight(w){};
};
class CEFIC
{
public:
	CEFIC();
	~CEFIC();
	
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void saveBackgroundModels(cv::Mat& image);
	void createLTPs(cv::Mat& source, cv::Mat& dest);
	void ltpsTogradient(cv::Mat& source, cv::Mat& dest);
	void doltp();
	void initBackgroundModel();
	void updateBackgroundModel();
	static inline double getDistance(ltps_node& a, const float* p);
	void updateLearningRate();
	inline double getExponentiallyRate(int t);
	inline void updateV_k(ltps_node& a, double x, double y, double rate);
	int getRcurr(int x, int y);
	int getRprev(int x, int y);
	void refineSegments(Mat& mask, Mat& dst);
	void opticalFlowFarneback();
	void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,double, const Scalar& color);
	double countMovePoint();
	void OpticalFlowPyrLK();
public:
	Mat pFrame;
	Mat LTPs;
	Mat prevFrame;
	bool isinit;
	vector<vector<vector<ltps_node>>> backgroundModel;
	vector<vector<double>> maxgradientlen;
	Mat eficForegroundMask;
	Mat prevEficForegroundMask;
	Mat updateRate;
	//Mat globalUnreliability;
	Mat localUnreliability;
	//Mat alpha_i_t;
	int exponentially_t;
	vector<double> erf;
	Mat flow, cflow;
	//Mat gray, prevGray, image;
	Mat LKimage;
	vector<vector<Point2f>> points;
};

