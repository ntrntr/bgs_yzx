#pragma once
class CAMBER
{
public:
	CAMBER();
	~CAMBER();
public:
	struct node
	{
		node(){};
		node(Vec3b f, int c)
		{
			data[0] = f[0];
			data[1] = f[1];
			data[2] = f[2];
			count = c;
			
		};
		bool operator < (const node& b)
		{
			return count < b.count;
		};
		double data[3];
		int count;
	};
public:
	void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate = 0);
	void getBackgroundImage(cv::Mat& image);
	void init();
	void colorJudgeAndUpdata(int x, int y, int level);
	int firsttime;
	inline void updateBk(double* bk, Vec3b& p);
	void getForegroundMask(Mat& fgmask);
	void postProcessAndGlobalControlProcedure();
private:
	double T_Y_min;
	double T_Y_max;
	double T_Cb_min;
	double T_Cb_max;
	double T_Cr_min;
	double T_Cr_max;
	double deltaY;
	double deltaCb;
	double deltaCr;
	double alpha;
	int thetaH;
	int thetaL;
	double gamma;
	int L0;
	int L1;
	int betaMax;
	int betaInc;
	int betaDec;
	int betaTH;
	double nMax;
	double nMin;
	const int K;
	double Tycbcr[3];
private:
	Mat pFrame;
	Mat pFrameDownSample;
	Mat foregroundMask;
	Mat foregroundMaskDownSample;
	Mat prevForegroundMask;
	Mat prevForegroundMaskDownSample;
	Size m_oImgSize;
	Size m_oImgSizeDownSample;
	int totalCount;
	int totalCountDownSample;
	vector<vector<node>> term;
	vector<vector<node>> TA;
	vector<vector<node>> TADOWNSAMPLE;
	vector<vector<node>> termDownSample;
	Mat An;
	Mat blinkingMap;
	Mat noiseMap;
	Mat prevnoiseMap;
	int downSampleUpdateRate;
	double distanceFactor;
	int forehitT;
	Mat finalMask;
};

