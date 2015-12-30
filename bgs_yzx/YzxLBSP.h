#pragma once
//! defines the default value for BackgroundSubtractorLBSP::m_fRelLBSPThreshold
#define BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD (0.365f)
//! defines the default value for BackgroundSubtractorLBSP::m_nLBSPThresholdOffset
#define BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD (0)
//! defines the default value for BackgroundSubtractorLOBSTER::m_nDescDistThreshold
#define BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD (4)
//! defines the default value for BackgroundSubtractorLOBSTER::m_nColorDistThreshold
#define BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD (30)
//! defines the default value for BackgroundSubtractorLOBSTER::m_nBGSamples
#define BGSLOBSTER_DEFAULT_NB_BG_SAMPLES (35)
//! defines the default value for BackgroundSubtractorLOBSTER::m_nRequiredBGSamples
#define BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES (2)
//! defines the default value for the learning rate passed to BackgroundSubtractorLOBSTER::operator()
#define BGSLOBSTER_DEFAULT_LEARNING_RATE (16)

#define DEFAULT_MEDIAN_BLUR_KERNEL_SIZE (9)

class CYzxLBSP
{
public:
	//CYzxLBSP();
	CYzxLBSP(
		float T_r = BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD,
		size_t lbspToffset = BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD,
		size_t descDistT = BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD,
		size_t colorDistT = BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD,
		size_t nBGSamples = BGSLOBSTER_DEFAULT_NB_BG_SAMPLES,
		size_t nRequiredBGSamples = BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES);
	~CYzxLBSP();
public:
	//fuc
	void initialize(const cv::Mat& oInitImg, const cv::Mat& oROI);
	void refreshModel(float fSamplesRefreshFrac, bool bForceFGUpdate = false);
	void operator()(const Mat& image, Mat& fgmask, double learningRate = BGSLOBSTER_DEFAULT_LEARNING_RATE);
	void getBackgroundImage(cv::OutputArray backgroundImage) const;
	void getBackgroundDescriptorsImage(cv::OutputArray backgroundDescImage) const;
	void computeRGBDescriptor(const Mat& oInputImg, const Vec3b& _ref, const int _x, const int _y, Vec3b& _t, Vec3w& _res);
	void computeSingleRGBDescriptor(const cv::Mat& oInputImg, const uchar _ref, const int _x, const int _y, const size_t _c, const size_t _t, ushort& _res);
	static inline size_t YZXL1dist(uchar a, uchar b)
	{
		return (size_t)abs((int)a - (int)b);
	}
	size_t get16bitsCount(uchar x);
	static inline void getGaussianPosition(int& x_sample, int& y_sample, const int x_orig, const int y_orig, const int border, const Size& imgsize);
	static inline size_t YZXLBSPhdist(ushort a, ushort b);
	inline static void computeRGBDescriptor(const cv::Mat& oInputImg, const uchar* const _ref, const int _x, const int _y, const size_t* const _t, ushort* _res);
	//for lbsp

public:
	void validateROI(cv::Mat& oROI);
	static const size_t PATCH_SIZE = 5;
	static const size_t DESC_SIZE = 2;
protected:
	struct PxInfoBase
	{
		int nImgCoord_Y;
		int nImgCoord_X;
		size_t nModelIdx;
	};
	const size_t m_nLBSPThresholdOffset;
	const float m_fRelLBSPThreshold;
	Mat m_oROI;
	Size m_oImgSize;
	size_t m_nImgChannels;
	int m_nImgType;
	size_t m_nTotPxCount, m_nTotRelevantPxCount;
	size_t m_nFrameIndex, m_nFramesSinceLastReset, m_nModelResetCooldown;
	size_t m_anLBSPThreshold_8bitLUT[UCHAR_MAX + 1];
	 
	vector<size_t> m_aPxIdxLUT;
	vector<PxInfoBase> m_aPxInfoLUT;
	const int m_nDefaultMedianBlurKernelSize;
	bool m_bInitialized;
	bool m_bAutoModelResetEnabled;
	bool m_bUsingMovingCamera;
	Mat m_oLastColorFrame;
	Mat m_oLastDescFrame;
	Mat m_oLastFGMask;
	//for yzxlbsp
protected:
	const size_t m_nColorDistThreshold;
	const size_t m_nDescDistThreshold;
	const size_t m_nBGSamples;
	const size_t m_nRequiredBGSamples;
	vector<Mat> m_voBGColorSamples;
	vector<Mat> m_voBGDescSamples;
public:
	Mat oNewBGROI;
	Mat oInputImg;
};

