#include "stdafx.h"
#include "YzxLBSP.h"
#include "GaussianRandom.h"
#include "YzxDistanceTool.h"
//#define LOBSTER
//CYzxLBSP::CYzxLBSP()
//{
//}


CYzxLBSP::CYzxLBSP(float T_r /*= BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD*/, size_t lbspToffset /*= BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD*/, size_t descDistT /*= BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD*/, size_t colorDistT /*= BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD*/, size_t nBGSamples /*= BGSLOBSTER_DEFAULT_NB_BG_SAMPLES*/, size_t nRequiredBGSamples /*= BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES*/) :m_fRelLBSPThreshold(T_r), m_nLBSPThresholdOffset(lbspToffset), m_nColorDistThreshold(colorDistT), m_nDescDistThreshold(descDistT), m_nBGSamples(nBGSamples), m_nRequiredBGSamples(nRequiredBGSamples),
m_nImgChannels(0),
m_nImgType(0),
m_nTotPxCount(0),
m_nTotRelevantPxCount(0),
m_nFrameIndex(SIZE_MAX),
m_nFramesSinceLastReset(0),
m_nModelResetCooldown(0),
//m_aPxIdxLUT(NULL),
//m_aPxInfoLUT(NULL),
m_nDefaultMedianBlurKernelSize(DEFAULT_MEDIAN_BLUR_KERNEL_SIZE),
m_bInitialized(false),
m_bAutoModelResetEnabled(true),
m_bUsingMovingCamera(false)
{
}

CYzxLBSP::~CYzxLBSP()
{
}

void CYzxLBSP::initialize(const cv::Mat& oInitImg, const cv::Mat& oROI)
{
	CV_Assert(oInitImg.isContinuous());
	if (oInitImg.type() == CV_8UC3)
	{
		vector<Mat> voInitImgChannels(3);
		split(oInitImg, voInitImgChannels);
		if (!countNonZero((voInitImgChannels[0] != voInitImgChannels[1]) | (voInitImgChannels[1] != voInitImgChannels[2])))
		{
			std::cout << std::endl << "\tBackgroundSubtractorLOBSTER : Warning, grayscale images should always be passed in CV_8UC1 format for optimal performance." << std::endl;
		}
	}
	//Mat oNewBGROI;
	oROI.copyTo(oNewBGROI);
	validateROI(oNewBGROI);
	const size_t nROIPxCount = (size_t)countNonZero(oNewBGROI);
	m_oROI = oNewBGROI;
	m_oImgSize = oInitImg.size();
	m_nImgType = oInitImg.type();
	m_nImgChannels = oInitImg.channels();
	m_nTotPxCount = m_oImgSize.area();
	m_nTotRelevantPxCount = nROIPxCount;
	m_nFrameIndex = 0;
	m_nFramesSinceLastReset = 0;
	m_nModelResetCooldown = 0;
	m_oLastFGMask.create(m_oImgSize, CV_8UC1);
	m_oLastFGMask = Scalar_<uchar>(0);
	m_oLastColorFrame.create(m_oImgSize, CV_8UC((int)m_nImgChannels));
	m_oLastColorFrame = Scalar_<uchar>::all(0);
	m_oLastDescFrame.create(m_oImgSize, CV_16UC((int)m_nImgChannels));
	m_oLastDescFrame = Scalar_<ushort>::all(0);
	//35 size, init
	m_voBGColorSamples.resize(m_nBGSamples);
	m_voBGDescSamples.resize(m_nBGSamples);
	for (int i = 0; i < m_nBGSamples; ++i)
	{
		m_voBGColorSamples[i].create(m_oImgSize, CV_8UC((int)m_nImgChannels));
		m_voBGColorSamples[i] = Scalar_<uchar>::all(0);
		m_voBGDescSamples[i].create(m_oImgSize, CV_16UC((int)m_nImgChannels));
		m_voBGDescSamples[i] = Scalar_<ushort>::all(0);
	}
	m_aPxIdxLUT.clear();
	m_aPxInfoLUT.clear();
	m_aPxIdxLUT.resize(m_nTotRelevantPxCount);
	m_aPxInfoLUT.resize(m_nTotPxCount);
	if (m_nImgChannels == 3)
	{
		for (int t = 0; t <= UCHAR_MAX; ++t)
		{
			//default m_nLBSPThresholdOffset = 0
			//delault m_fRelLBSPThreshold = 0.365 , Tr
			m_anLBSPThreshold_8bitLUT[t] = (uchar)(t * m_fRelLBSPThreshold + m_nLBSPThresholdOffset);
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
#ifdef LOBSTER
		for (int nPxIter = 0, nModelIter = 0; nPxIter < m_nTotPxCount; ++nPxIter)
		{
			//m_oROI
			if (m_oROI.data[nPxIter])
			{
				m_aPxIdxLUT[nModelIter] = nPxIter;
				m_aPxInfoLUT[nPxIter].nImgCoord_Y = (int)nPxIter / m_oImgSize.width;
				m_aPxInfoLUT[nPxIter].nImgCoord_X = (int)nPxIter%m_oImgSize.width;
				m_aPxInfoLUT[nPxIter].nModelIdx = nModelIter;
				const size_t nPxRGBIter = nPxIter * 3;
				const size_t nDescRGBIter = nPxRGBIter * 2;
				for (size_t c = 0; c < 3; ++c) {
					m_oLastColorFrame.data[nPxRGBIter + c] = oInitImg.data[nPxRGBIter + c];
					computeSingleRGBDescriptor(oInitImg, oInitImg.data[nPxRGBIter + c], m_aPxInfoLUT[nPxIter].nImgCoord_X, m_aPxInfoLUT[nPxIter].nImgCoord_Y, c, m_anLBSPThreshold_8bitLUT[oInitImg.data[nPxRGBIter + c]], ((ushort*)(m_oLastDescFrame.data + nDescRGBIter))[c]);
				}
				++nModelIter;
			}
		}
#else
		int count = 0;
		for (int i = 0; i < m_oImgSize.height; ++i)
		{
			uchar* pROI = m_oROI.ptr<uchar>(i);
			for (int j = 0; j < m_oImgSize.width; ++j)
			{
				if (pROI[j])
				{
					m_aPxIdxLUT[count] = j + i * m_oImgSize.width;
					m_aPxInfoLUT[j + i * m_oImgSize.width].nImgCoord_Y = i;
					m_aPxInfoLUT[j + i * m_oImgSize.width].nImgCoord_X = j;
					m_aPxInfoLUT[j + i * m_oImgSize.width].nModelIdx = count;
					Vec3b tmp(
						m_anLBSPThreshold_8bitLUT[oInitImg.at<Vec3b>(i, j)[0]],
						m_anLBSPThreshold_8bitLUT[oInitImg.at<Vec3b>(i, j)[1]],
						m_anLBSPThreshold_8bitLUT[oInitImg.at<Vec3b>(i, j)[2]]);
					m_oLastColorFrame.at<Vec3b>(i, j) = oInitImg.at<Vec3b>(i, j);
					computeRGBDescriptor(oInitImg, oInitImg.at<Vec3b>(i, j), j, i, tmp,
						m_oLastDescFrame.at<Vec3w>(i, j)
						);
					++count;
				}
			}

		}
#endif
	}
	//channels == 1
	else
	{

	}
	m_bInitialized = true;
	refreshModel(1.0f);
}

void CYzxLBSP::refreshModel(float fSamplesRefreshFrac, bool bForceFGUpdate /*= false*/)
{
	const size_t nModelsToRefresh = fSamplesRefreshFrac < 1.0f ? (size_t)(fSamplesRefreshFrac * m_nBGSamples) : m_nBGSamples;
	const size_t nRefreshStartPos = fSamplesRefreshFrac < 1.0f ? rand()%m_nBGSamples : 0;
	if (m_nImgChannels == 3)
	{
		for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter)
		{
			const size_t nPxIter = m_aPxIdxLUT[nModelIter];
			if (bForceFGUpdate || !m_oLastFGMask.data[nPxIter])
			{
				for (int nCurrModelIdx = nRefreshStartPos; nCurrModelIdx < nRefreshStartPos + nModelsToRefresh; ++nCurrModelIdx)
				{
					int nSampleImgCoord_Y, nSampleImgCoord_X;
#ifdef LOBSTER
					getRandSamplePosition(nSampleImgCoord_X, nSampleImgCoord_Y, m_aPxInfoLUT[nPxIter].nImgCoord_X, m_aPxInfoLUT[nPxIter].nImgCoord_Y, PATCH_SIZE / 2, m_oImgSize);
					const size_t nSamplePxIdx = m_oImgSize.width * nSampleImgCoord_Y + nSampleImgCoord_X;
					if (bForceFGUpdate || !m_oLastFGMask.data[nSamplePxIdx])
					{
						const size_t nCurrRealModelIdx = nCurrModelIdx % m_nBGSamples;
						for (size_t c = 0; c < 3; ++c)
						{
							m_voBGColorSamples[nCurrRealModelIdx].data[nPxIter * 3 + c] = m_oLastColorFrame.data[nSamplePxIdx * 3 + c];
							*((ushort*)(m_voBGDescSamples[nCurrRealModelIdx].data + 2 * (nPxIter * 3 + c))) = *((ushort*)(m_oLastDescFrame.data + 2 * (nSamplePxIdx * 3 + c)));
						}
					}
#else
					getGaussianPosition(nSampleImgCoord_X, nSampleImgCoord_Y, m_aPxInfoLUT[nPxIter].nImgCoord_X, m_aPxInfoLUT[nPxIter].nImgCoord_Y, PATCH_SIZE / 2, m_oImgSize);
					const size_t nSamplePxIdx = m_oImgSize.width * nSampleImgCoord_Y + nSampleImgCoord_X;
					if (bForceFGUpdate || !m_oLastFGMask.data[nSamplePxIdx])
					{
						const size_t nCurrRealModelIdx = nCurrModelIdx % m_nBGSamples;
						m_voBGColorSamples[nCurrRealModelIdx].at<Vec3b>(m_aPxInfoLUT[nPxIter].nImgCoord_Y, m_aPxInfoLUT[nPxIter].nImgCoord_X) = m_oLastColorFrame.at<Vec3b>(nSampleImgCoord_Y, nSampleImgCoord_X);
						m_voBGDescSamples[nCurrRealModelIdx].at<Vec3w>(m_aPxInfoLUT[nPxIter].nImgCoord_Y, m_aPxInfoLUT[nPxIter].nImgCoord_X) = m_oLastDescFrame.at<Vec3w>(nSampleImgCoord_Y, nSampleImgCoord_X);
						/*m_voBGColorSamples[nCurrRealModelIdx].at<Vec3b>(nPxIter / m_oImgSize.width, nPxIter % m_oImgSize.width) = m_oLastColorFrame.at<Vec3b>(nPxIter / m_oImgSize.width, nPxIter % m_oImgSize.width);
						m_voBGDescSamples[nCurrRealModelIdx].at<Vec3w>(nPxIter / m_oImgSize.width, nPxIter % m_oImgSize.width) = m_oLastDescFrame.at<Vec3w>(nPxIter / m_oImgSize.width, nPxIter % m_oImgSize.width);*/
					}
#endif
				}
			}
		}
	}
	//m_nImgchannels == 1
	else
	{

	}
}


void CYzxLBSP::operator()(const Mat& image, Mat& fgmask, double learningRate /*= BGSLOBSTER_DEFAULT_LEARNING_RATE*/)
{
	//m_voBGColorSamples[0].copyTo(fgmask);
	fgmask.create(m_oImgSize, CV_8UC1);
	fgmask = Scalar_<uchar>(0);
	image.copyTo(oInputImg);
	const size_t nLearningRate = (size_t)ceil(learningRate);
	if (m_nImgChannels == 3)
	{
		//m_nDescDistThreshold default = 4 ,channels = 3, 4 * 3
		const size_t nCurrDescDistThreshold = m_nDescDistThreshold * 3;
		//m_nColorDistThreshold default = 30
		const size_t nCurrColorDistThreshold = m_nColorDistThreshold * 3;
		const size_t nCurrSCDescDistThreshold = nCurrDescDistThreshold / 2;
		const size_t nCurrSCColorDistThreshold = nCurrColorDistThreshold / 2;
		const size_t desc_row_step = m_voBGDescSamples[0].step[0];
		const size_t img_row_step = m_voBGColorSamples[0].step[0];
		for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter)
		{
			const size_t nPxIter = m_aPxIdxLUT[nModelIter];
			const int nCurrImgCoord_X = m_aPxInfoLUT[nPxIter].nImgCoord_X;
			const int nCurrImgCoord_Y = m_aPxInfoLUT[nPxIter].nImgCoord_Y;
			//uchar 8u
			const size_t nPxIterRGB = nPxIter * 3;
			//float 16F
			const size_t nDescIterRGB = nPxIterRGB * 2;
			const uchar* const anCurrColor = oInputImg.data + nPxIterRGB;
			size_t nGoodSamplesCount = 0, nModelIdx = 0;
			ushort anCurrInputDesc[3];
			while (nGoodSamplesCount < m_nRequiredBGSamples && nModelIdx < m_nBGSamples)
			{
				const ushort* const anBGDesc = (ushort*)(m_voBGDescSamples[nModelIdx].data + nDescIterRGB);
				const uchar* const anBGColor = (uchar*)(m_voBGColorSamples[nModelIdx].data + nPxIterRGB);
				size_t nTotColorDist = 0;
				size_t nTotDescDist = 0;
				bool flag = true;
				for (size_t c = 0; c < 3; ++c)
				{
					const size_t nColorDist = YZXL1dist(anCurrColor[c], anBGColor[c]);
					if (nColorDist > nCurrSCColorDistThreshold)
					{
						flag = false;
						break;
					}
					computeSingleRGBDescriptor(oInputImg, anBGColor[c], nCurrImgCoord_X, nCurrImgCoord_Y, c, m_anLBSPThreshold_8bitLUT[anBGColor[c]], anCurrInputDesc[c]);
					const size_t nDescDist = YZXhdist(anCurrInputDesc[c], anBGDesc[c]);
					//const size_t nDescDist = YZXLBSPhdist(anCurrInputDesc[c], anBGDesc[c]);
					if (nDescDist > nCurrSCDescDistThreshold)
					{
						flag = false;
						break;
					}
					nTotColorDist += nColorDist;
					nTotDescDist += nDescDist;
				}
				if (flag && nTotColorDist <= nCurrColorDistThreshold && nTotDescDist <= nCurrDescDistThreshold)
				{
					++nGoodSamplesCount;
				}
				++nModelIdx;
			}
			if (nGoodSamplesCount < m_nRequiredBGSamples)
			{
				fgmask.data[nPxIter] = UCHAR_MAX;
			}
			else
			{
				//update
				if ((rand()%nLearningRate) == 0)
				{
					const size_t nSampleModelIdx = rand() % m_nBGSamples;
					ushort* anRandInputDesc = (ushort*)(m_voBGDescSamples[nSampleModelIdx].data + nDescIterRGB);
					const size_t anCurrIntraLBSPThresholds[3] = {
						m_anLBSPThreshold_8bitLUT[anCurrColor[0]],
						m_anLBSPThreshold_8bitLUT[anCurrColor[1]],
						m_anLBSPThreshold_8bitLUT[anCurrColor[2]]
					};
					computeRGBDescriptor(oInputImg, anCurrColor, nCurrImgCoord_X, nCurrImgCoord_Y, anCurrIntraLBSPThresholds, anRandInputDesc);
					for (size_t c = 0; c < 3;++c)
					{
						*(m_voBGColorSamples[nSampleModelIdx].data + nPxIterRGB + c) = anCurrColor[c];
					}
				}
				//updateneighbour
				if ((rand() % nLearningRate) == 0)
				{
					int nSampleImgCoord_Y, nSampleImgCoord_X;
					getRandNeighborPosition_3x3(nSampleImgCoord_X, nSampleImgCoord_Y, nCurrImgCoord_X, nCurrImgCoord_Y, PATCH_SIZE / 2, m_oImgSize);
					const size_t nSampleModelIdx = rand() % m_nBGSamples;
					//float 2, channel 3 2*3=6
					ushort* anRandInputDesc = (ushort*)(m_voBGDescSamples[nSampleModelIdx].data + desc_row_step * nSampleImgCoord_Y + 6 * nSampleImgCoord_X);
					const size_t anCurrIntraLBSPThresholds[3] = {
						m_anLBSPThreshold_8bitLUT[anCurrColor[0]],
						m_anLBSPThreshold_8bitLUT[anCurrColor[1]],
						m_anLBSPThreshold_8bitLUT[anCurrColor[2]]
					};
					computeRGBDescriptor(oInputImg, anCurrColor, nCurrImgCoord_X, nCurrImgCoord_Y, anCurrIntraLBSPThresholds, anRandInputDesc);
					for (size_t c = 0; c < 3; ++c)
					{
						*(m_voBGColorSamples[nSampleModelIdx].data + img_row_step * nSampleImgCoord_Y + 3 * nSampleImgCoord_X + c) = anCurrColor[c];
					}
				}
			}
		}
	}
	else
	{
		//todo channels == 1
	}
	//medianBlur(fgmask, m_oLastFGMask, m_nDefaultMedianBlurKernelSize);
	m_oLastFGMask.copyTo(fgmask);
}

void CYzxLBSP::getBackgroundImage(cv::OutputArray backgroundImage) const
{
	Mat res = Mat::zeros(m_oImgSize, CV_32FC((int)(m_nImgChannels)));
	for (size_t s = 0; s < m_nBGSamples; ++s)
	{
		for (int y = 0; y < m_oImgSize.height; ++y)
		{
			for (int x = 0; x < m_oImgSize.width; ++x)
			{
				const size_t idx_nimg = m_voBGColorSamples[s].step.p[0] * y + m_voBGColorSamples[s].step.p[1] * x;
				const size_t idx_flt32 = idx_nimg * 4;
				float* oAvgBgImgPtr = (float*)(res.data + idx_flt32);
				const uchar* const oBGImgPtr = m_voBGColorSamples[s].data + idx_nimg;
				for (size_t c = 0; c < m_nImgChannels; ++c)
					oAvgBgImgPtr[c] += ((float)oBGImgPtr[c]) / m_nBGSamples;
			}
		}
	}
	res.convertTo(backgroundImage, CV_8U);
}

void CYzxLBSP::getBackgroundDescriptorsImage(cv::OutputArray backgroundDescImage) const
{

}

void CYzxLBSP::computeRGBDescriptor(const Mat& oInputImg, const Vec3b& _ref, const int _x, const int _y, Vec3b& _t, Vec3w& _res)
{
#define _val(x,y,n) _data[_step_row*(_y+y)+3*(_x+x)+n]
	const size_t _step_row = oInputImg.step[0];
	const uchar* const _data = oInputImg.data;
	for (int n = 0; n < 3; ++n)
	{
		_res[n] =
			((YZXL1dist(_val(-1, 1, n), _ref[n]) > _t[n]) << 15)
			+ ((YZXL1dist(_val(1, -1, n), _ref[n]) > _t[n]) << 14)
			+ ((YZXL1dist(_val(1, 1, n), _ref[n]) > _t[n]) << 13)
			+ ((YZXL1dist(_val(-1, -1, n), _ref[n]) > _t[n]) << 12)
			+ ((YZXL1dist(_val(1, 0, n), _ref[n]) > _t[n]) << 11)
			+ ((YZXL1dist(_val(0, -1, n), _ref[n]) > _t[n]) << 10)
			+ ((YZXL1dist(_val(-1, 0, n), _ref[n]) > _t[n]) << 9)
			+ ((YZXL1dist(_val(0, 1, n), _ref[n]) > _t[n]) << 8)
			+ ((YZXL1dist(_val(-2, -2, n), _ref[n]) > _t[n]) << 7)
			+ ((YZXL1dist(_val(2, 2, n), _ref[n]) > _t[n]) << 6)
			+ ((YZXL1dist(_val(2, -2, n), _ref[n]) > _t[n]) << 5)
			+ ((YZXL1dist(_val(-2, 2, n), _ref[n]) > _t[n]) << 4)
			+ ((YZXL1dist(_val(0, 2, n), _ref[n]) > _t[n]) << 3)
			+ ((YZXL1dist(_val(0, -2, n), _ref[n]) > _t[n]) << 2)
			+ ((YZXL1dist(_val(2, 0, n), _ref[n]) > _t[n]) << 1)
			+ ((YZXL1dist(_val(-2, 0, n), _ref[n]) > _t[n]));
	}
}

void CYzxLBSP::computeRGBDescriptor(const cv::Mat& oInputImg, const uchar* const _ref, const int _x, const int _y, const size_t* const _t, ushort* _res)
{
	const size_t _step_row = oInputImg.step.p[0];
	const uchar* const _data = oInputImg.data;
#define _val(x,y,n) _data[_step_row*(_y+y)+3*(_x+x)+n]
	for (int n = 0; n<3; ++n) {
		_res[n] = ((YZXL1dist(_val(-1, 1, n), _ref[n]) > _t[n]) << 15)
			+ ((YZXL1dist(_val(1, -1, n), _ref[n]) > _t[n]) << 14)
			+ ((YZXL1dist(_val(1, 1, n), _ref[n]) > _t[n]) << 13)
			+ ((YZXL1dist(_val(-1, -1, n), _ref[n]) > _t[n]) << 12)
			+ ((YZXL1dist(_val(1, 0, n), _ref[n]) > _t[n]) << 11)
			+ ((YZXL1dist(_val(0, -1, n), _ref[n]) > _t[n]) << 10)
			+ ((YZXL1dist(_val(-1, 0, n), _ref[n]) > _t[n]) << 9)
			+ ((YZXL1dist(_val(0, 1, n), _ref[n]) > _t[n]) << 8)
			+ ((YZXL1dist(_val(-2, -2, n), _ref[n]) > _t[n]) << 7)
			+ ((YZXL1dist(_val(2, 2, n), _ref[n]) > _t[n]) << 6)
			+ ((YZXL1dist(_val(2, -2, n), _ref[n]) > _t[n]) << 5)
			+ ((YZXL1dist(_val(-2, 2, n), _ref[n]) > _t[n]) << 4)
			+ ((YZXL1dist(_val(0, 2, n), _ref[n]) > _t[n]) << 3)
			+ ((YZXL1dist(_val(0, -2, n), _ref[n]) > _t[n]) << 2)
			+ ((YZXL1dist(_val(2, 0, n), _ref[n]) > _t[n]) << 1)
			+ ((YZXL1dist(_val(-2, 0, n), _ref[n]) > _t[n]));
	}
}

void CYzxLBSP::computeSingleRGBDescriptor(const cv::Mat& oInputImg, const uchar _ref, const int _x, const int _y, const size_t _c, const size_t _t, ushort& _res)
{
#define _val(x,y,n) _data[_step_row*(_y+y)+3*(_x+x)+n]
	const size_t _step_row = oInputImg.step.p[0];
	const uchar* const _data = oInputImg.data;
	_res = ((YZXL1dist(_val(-1, 1, _c), _ref) > _t) << 15)
		+ ((YZXL1dist(_val(1, -1, _c), _ref) > _t) << 14)
		+ ((YZXL1dist(_val(1, 1, _c), _ref) > _t) << 13)
		+ ((YZXL1dist(_val(-1, -1, _c), _ref) > _t) << 12)
		+ ((YZXL1dist(_val(1, 0, _c), _ref) > _t) << 11)
		+ ((YZXL1dist(_val(0, -1, _c), _ref) > _t) << 10)
		+ ((YZXL1dist(_val(-1, 0, _c), _ref) > _t) << 9)
		+ ((YZXL1dist(_val(0, 1, _c), _ref) > _t) << 8)
		+ ((YZXL1dist(_val(-2, -2, _c), _ref) > _t) << 7)
		+ ((YZXL1dist(_val(2, 2, _c), _ref) > _t) << 6)
		+ ((YZXL1dist(_val(2, -2, _c), _ref) > _t) << 5)
		+ ((YZXL1dist(_val(-2, 2, _c), _ref) > _t) << 4)
		+ ((YZXL1dist(_val(0, 2, _c), _ref) > _t) << 3)
		+ ((YZXL1dist(_val(0, -2, _c), _ref) > _t) << 2)
		+ ((YZXL1dist(_val(2, 0, _c), _ref) > _t) << 1)
		+ ((YZXL1dist(_val(-2, 0, _c), _ref) > _t));
}

size_t CYzxLBSP::get16bitsCount(uchar x)
{
	size_t res = 0;
	while (x)
	{
		res += (x & 1);
		x = x >> 1;
	}
	return res;
}

void CYzxLBSP::getGaussianPosition(int& x_sample, int& y_sample, const int x_orig, const int y_orig, const int border, const Size& imgsize)
{
	static const int s_nSamplesInitPatternWidth = 7;
	static const int s_nSamplesInitPatternHeight = 7;
	static const int s_nSamplesInitPatternTot = 512;
	static const int s_anSamplesInitPattern[s_nSamplesInitPatternHeight][s_nSamplesInitPatternWidth] = {
		{ 2, 4, 6, 7, 6, 4, 2, },
		{ 4, 8, 12, 14, 12, 8, 4, },
		{ 6, 12, 21, 25, 21, 12, 6, },
		{ 7, 14, 25, 28, 25, 14, 7, },
		{ 6, 12, 21, 25, 21, 12, 6, },
		{ 4, 8, 12, 14, 12, 8, 4, },
		{ 2, 4, 6, 7, 6, 4, 2, },
	};
	int r = 1 + rand() % s_nSamplesInitPatternTot;
	int flag = 0;
	for (x_sample = 0; x_sample < s_nSamplesInitPatternWidth; ++x_sample)
	{
		for (y_sample = 0; y_sample < s_nSamplesInitPatternHeight; ++y_sample)
		{
			r -= s_anSamplesInitPattern[y_sample][x_sample];
			if (r <= 0)
			{
				flag = 1;
				break;
			}
		}
		if (flag == 1)
		{
			break;
		}
	}
	x_sample += x_orig - s_nSamplesInitPatternWidth / 2;
	y_sample += y_orig - s_nSamplesInitPatternHeight / 2;
	if (x_sample < border)
	{
		x_sample = border;
	}
	else if (x_sample >= imgsize.width - border)
	{
		x_sample = imgsize.width - border - 1;
	}
	if (y_sample < border)
	{
		y_sample = border;
	}
	else if (y_sample >= imgsize.height - border)
	{
		y_sample = imgsize.height - border - 1;
	}
}

size_t CYzxLBSP::YZXLBSPhdist(ushort a, ushort b)
{
	ushort x = (a^b);
	size_t res = 0;
	while (x)
	{
		res += (x & 1);
		x = x >> 1;
	}
	return res;
}

void CYzxLBSP::validateROI(cv::Mat& oROI)
{
	Mat oROI_new(oROI.size(), CV_8UC1, Scalar_<uchar>(0));
	size_t nBorderSize = PATCH_SIZE / 2;
	Rect nROI_inner(nBorderSize, nBorderSize, oROI.cols - nBorderSize * 2, oROI.rows - nBorderSize * 2);
	Mat(oROI, nROI_inner).copyTo(Mat(oROI_new, nROI_inner));
	oROI = oROI_new;
}
