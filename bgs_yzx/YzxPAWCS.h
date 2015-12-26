#pragma once
class CYzxPAWCS
{
public:
	CYzxPAWCS();
	~CYzxPAWCS();
protected:
	struct LocalWordBase;
	struct GlobalWordBase;
public:
	void init(const Mat& oInitImg, const Mat& oRoi);
	void refreshModel(int nBaseOccCount, float fOccDecrFrac, bool bForceFGUpdate = false);
	void operator()(InputArray image, OutputArray fgmask, double learningRateOverride = 0);
	void getBackgroundImage(OutputArray backgroundImage) const;
	void getBackgroundDescriptorsImage(OutputArray backgroundDescImage) const;
	void CleanupDictionaries();
	float GetLocalWordWeight(const LocalWordBase* w, size_t nCurFrame, size_t nOffset);
	float GetGlobalWordWeight(const GlobalWordBase* w);
protected:
	template<int nChannels>
	struct ColorLBSPFeature
	{
		uchar anColor[nChannels];
		ushort anDesc[nChannels];
	};
	struct LocalWordBase
	{
		size_t nFirstOcc;
		size_t nLastOcc;
		size_t nOccurrences;
	};

	template<typename T>
	struct LocalWord:LocalWordBase
	{
		T oFeature;
	};

	struct GlobalWordBase
	{
		float fLatestWeight;
		Mat oSpatioOccMap;
		uchar nDescBITS;
	};

	template<typename T>
	struct GlobalWord:GlobalWordBase
	{
		T oFeature;
	};
	typedef LocalWord<ColorLBSPFeature<1>> LocalWord_1ch;
	typedef LocalWord<ColorLBSPFeature<3>> LocalWord_3ch;
	typedef GlobalWord<ColorLBSPFeature<1>> GlobalWord_1ch;
	typedef GlobalWord<ColorLBSPFeature<3>> GlobalWord_3ch;

	struct PxInfoBase
	{
		int nImgCoord_Y;
		int nImgCoord_X;
		size_t nModelIdx;

	};
	struct PxInfo_PAWCS:PxInfoBase
	{
		size_t nGlobalWordMapLookupIdx;
		GlobalWordBase** apGlobalDictSortLut;
	};
	const size_t m_nR_x;
	const size_t m_nLBSPThresholdOffset;
	size_t m_nMaxLocalWords, m_nCurLocalWords;
	size_t m_nMaxGlobalWords, m_mCurGlobalWords;
	const size_t m_nSamplesForMovingAvgs;
	float m_fLastNonFlatRegionRatio;
	int m_nMedianBlurKernelSize;
	Size m_oDownSampledFrameSize_MotionAnalysis;
	Size m_oDownSampledFrameSize_GlobalWordLookup;
	Mat m_oDownSampledROI_MotionAnalysis;
	size_t m_nDownSampledROIPxCount;
	size_t m_nLocalWordWeightOffset;
	LocalWordBase** m_apLocalWordDict;
	LocalWord_3ch* m_aLocalWordList_3ch, *m_pLocalWordListIter_3ch;
	GlobalWordBase** m_apGlobalWordDict;
	GlobalWord_3ch* m_aGlobalWordList_3ch, *m_pGlobalWordListIter_3ch;
	PxInfo_PAWCS* m_aPxInfoLUT_PAWCS;

	Mat m_oIlluminationUpdateRegionMask;
	Mat m_oT_x;
	Mat m_oR_x;
	Mat m_oV_x;
	Mat m_oD_min_LT, m_oD_min_ST;
	Mat m_oDownSampled_D_min_LT, m_oDownSampled_D_min_ST;
	Mat m_oMeanRawSegmResFrame_LT, m_oMeanRawSegmResFrame_ST;
	Mat m_oMeanFinalSegmResFrame_LT, m_oMeanFinalSegmResFrame_ST;
	Mat m_oUnstableRegionMask;
	Mat m_oZ_x;

	Mat m_oDownSampledFrame_MotionAnalysis;
	Mat m_oLastRawFGMask;
};

