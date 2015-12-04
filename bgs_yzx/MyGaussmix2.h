#pragma once
#include "precomp.hpp"
using namespace cv;
// default parameters of gaussian background detection algorithm
static const int defaultHistory2 = 500; // Learning rate; alpha = 1/defaultHistory2
static const float defaultVarThreshold2 = 4.0f*4.0f;
static const int defaultNMixtures2 = 5; // maximal number of Gaussians in mixture
static const float defaultBackgroundRatio2 = 0.9f; // threshold sum of weights for background test
static const float defaultVarThresholdGen2 = 3.0f*3.0f;
static const float defaultVarInit2 = 15.0f; // initial variance for new components
static const float defaultVarMax2 = 5*defaultVarInit2;
static const float defaultVarMin2 = 4.0f;

// additional parameters
static const float defaultfCT2 = 0.05f; // complexity reduction prior constant 0 - no reduction of number of components
static const unsigned char defaultnShadowDetection2 = (unsigned char)255; // value to use in the segmentation mask for shadows, set 0 not to do shadow detection
static const float defaultfTau = 0.5f; // Tau - shadow threshold, see the paper for explanation
struct GaussBGStatModel2Params
{
	int nWidth;
	int nHeight;
	int nND; //image channels
	bool bPostFiltering;
	double minArea;
	bool bInit;
	float fAlphaT;
	float fTb;
	float fTg;
	float fTB;
	//foreground
	float fVarInit;
	float fVarMax;
	float fVarMin;
	float fCT;

	int nM;
	bool bShadowDetection;
	unsigned char nShadowDetection;
	float fTau;
};
struct GMM
{
	float weight;
	float variance;
};


class MyGaussmix2: public BackgroundSubtractor
{
public:
	MyGaussmix2(void);
	MyGaussmix2(int _history,  float _varThreshold, bool _bShadowDetection);
	~MyGaussmix2(void);
public:
	// shadow detection performed per pixel
	// should work for rgb data, could be usefull for gray scale and depth data as well
	static CV_INLINE bool
		detectShadowGMM(const float* data, int nchannels, int nmodes,
		const GMM* gmm, const float* mean,
		float Tb, float TB, float tau);

	//  [11/20/2015 yzx]
	virtual	void initialize(Size _frameSize, int _frameType);
	virtual void operator()(InputArray _image, OutputArray _fgmask, double learningRate);
	virtual void getBackgroundImage(OutputArray backgroundImage) const;
	//virtual AlgorithmInfo* info() const;
	void saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count);
public:
	Size frameSize;
	int frameType;
	Mat bgmodel;
	Mat bgmodelUsedModes;
	int nframes;
	int history;
	int nmixtures;
	double varThreshold;
	float backgroundRatio;
	float varThresholdGen;
	float fVarInit;
	float fVarMin;
	float fVarMax;
	float fCT;
	bool bShadowDetection;
	unsigned char nShadowDetection;
	float fTau;
};

struct MOG2Invoker : ParallelLoopBody
{
	//parallel_for_(Range(0, image.rows), 
	//	MOG2Invoker(image,
	//	fgmask, 
	//	(GMM*)bgmodel.data, 
	//	(float*)(bgmodel.data + sizeof(GMM) * nmixtures * image.rows * image.cols),
	//	bgmodelUsedModes.data,
	//	nmixtures,
	//	(float)learningRate, 
	//	(float)varThreshold, 
	//	backgroundRatio, 
	//	varThresholdGen, 
	//	fVarInit, 
	//	fVarMin, 
	//	fVarMax, 
	//	float(-learningRate * fCT), 
	//	fTau, 
	//	bShadowDetection, 
	//	nShadowDetection));
	MOG2Invoker(const Mat& _src, 
		Mat& _dst,
		GMM* _gmm, 
		float* _mean,
		uchar* _modesUsed,
		int _nmixtures, 
		float _alphaT,
		float _Tb, 
		float _TB, 
		float _Tg,
		float _varInit, 
		float _varMin, 
		float _varMax,
		float _prune, 
		float _tau, 
		bool _detectShadows,
		uchar _shadowVal)
	{
		src = &_src;
		dst = &_dst;
		gmm0 = _gmm;
		mean0 = _mean;
		modesUsed0 = _modesUsed;
		nmixtures = _nmixtures;
		alphaT = _alphaT;
		Tb = _Tb;
		TB = _TB;
		Tg = _Tg;
		varInit = _varInit;
		varMin = MIN(_varMin, _varMax);
		varMax = MAX(_varMin, _varMax);
		prune = _prune;
		tau = _tau;
		detectShadows = _detectShadows;
		shadowVal = _shadowVal;

		cvtfunc = src->depth() != CV_32F ? getConvertFunc(src->depth(), CV_32F) : 0;
	}

	void operator()(const Range& range) const
	{
		int y0 = range.start, y1 = range.end;
		int ncols = src->cols, nchannels = src->channels();
		AutoBuffer<float> buf(src->cols*nchannels);
		float alpha1 = 1.f - alphaT;
		float dData[CV_CN_MAX];

		for( int y = y0; y < y1; y++ )
		{
			const float* data = buf;
			if( cvtfunc )
				cvtfunc( src->ptr(y), src->step, 0, 0, (uchar*)data, 0, Size(ncols*nchannels, 1), 0);
			else
				data = src->ptr<float>(y);

			float* mean = mean0 + ncols*nmixtures*nchannels*y;
			GMM* gmm = gmm0 + ncols*nmixtures*y;
			uchar* modesUsed = modesUsed0 + ncols*y;
			uchar* mask = dst->ptr(y);

			for( int x = 0; x < ncols; x++, data += nchannels, gmm += nmixtures, mean += nmixtures*nchannels )
			{
				//calculate distances to the modes (+ sort)
				//here we need to go in descending order!!!
				bool background = false;//return value -> true - the pixel classified as background

				//internal:
				bool fitsPDF = false;//if it remains zero a new GMM mode will be added
				int nmodes = modesUsed[x], nNewModes = nmodes;//current number of modes in GMM
				float totalWeight = 0.f;

				float* mean_m = mean;

				//////
				//go through all modes
				for( int mode = 0; mode < nmodes; mode++, mean_m += nchannels )
				{
					float weight = alpha1*gmm[mode].weight + prune;//need only weight if fit is found
					int swap_count = 0;
					////
					//fit not found yet
					if( !fitsPDF )
					{
						//check if it belongs to some of the remaining modes
						float var = gmm[mode].variance;

						//calculate difference and distance
						float dist2;

						if( nchannels == 3 )
						{
							dData[0] = mean_m[0] - data[0];
							dData[1] = mean_m[1] - data[1];
							dData[2] = mean_m[2] - data[2];
							dist2 = dData[0]*dData[0] + dData[1]*dData[1] + dData[2]*dData[2];
						}
						else
						{
							dist2 = 0.f;
							for( int c = 0; c < nchannels; c++ )
							{
								dData[c] = mean_m[c] - data[c];
								dist2 += dData[c]*dData[c];
							}
						}

						//background? - Tb - usually larger than Tg
						if( totalWeight < TB && dist2 < Tb*var )
							background = true;

						//check fit
						if( dist2 < Tg*var )
						{
							/////
							//belongs to the mode
							fitsPDF = true;

							//update distribution

							//update weight
							weight += alphaT;
							float k = alphaT/weight;

							//update mean
							for( int c = 0; c < nchannels; c++ )
								mean_m[c] -= k*dData[c];

							//update variance
							float varnew = var + k*(dist2-var);
							//limit the variance
							varnew = MAX(varnew, varMin);
							varnew = MIN(varnew, varMax);
							gmm[mode].variance = varnew;

							//sort
							//all other weights are at the same place and
							//only the matched (iModes) is higher -> just find the new place for it
							for( int i = mode; i > 0; i-- )
							{
								//check one up
								if( weight < gmm[i-1].weight )
									break;

								swap_count++;
								//swap one up
								std::swap(gmm[i], gmm[i-1]);
								for( int c = 0; c < nchannels; c++ )
									std::swap(mean[i*nchannels + c], mean[(i-1)*nchannels + c]);
							}
							//belongs to the mode - bFitsPDF becomes 1
							/////
						}
					}//!bFitsPDF)

					//check prune
					if( weight < -prune )
					{
						weight = 0.0;
						nmodes--;
					}

					gmm[mode-swap_count].weight = weight;//update weight by the calculated value
					totalWeight += weight;
				}
				//go through all modes
				//////

				//renormalize weights
				totalWeight = 1.f/totalWeight;
				for( int mode = 0; mode < nmodes; mode++ )
				{
					gmm[mode].weight *= totalWeight;
				}

				nmodes = nNewModes;

				//make new mode if needed and exit
				if( !fitsPDF )
				{
					// replace the weakest or add a new one
					int mode = nmodes == nmixtures ? nmixtures-1 : nmodes++;

					if (nmodes==1)
						gmm[mode].weight = 1.f;
					else
					{
						gmm[mode].weight = alphaT;

						// renormalize all other weights
						for( int i = 0; i < nmodes-1; i++ )
							gmm[i].weight *= alpha1;
					}

					// init
					for( int c = 0; c < nchannels; c++ )
						mean[mode*nchannels + c] = data[c];

					gmm[mode].variance = varInit;

					//sort
					//find the new place for it
					for( int i = nmodes - 1; i > 0; i-- )
					{
						// check one up
						if( alphaT < gmm[i-1].weight )
							break;

						// swap one up
						std::swap(gmm[i], gmm[i-1]);
						for( int c = 0; c < nchannels; c++ )
							std::swap(mean[i*nchannels + c], mean[(i-1)*nchannels + c]);
					}
				}

				//set the number of modes
				modesUsed[x] = uchar(nmodes);
				mask[x] = background ? 0 :
					detectShadows && MyGaussmix2::detectShadowGMM(data, nchannels, nmodes, gmm, mean, Tb, TB, tau) ?
shadowVal : 255;
			}

		}
	}

	const Mat* src;
	Mat* dst;
	GMM* gmm0;
	float* mean0;
	uchar* modesUsed0;

	int nmixtures;
	float alphaT, Tb, TB, Tg;
	float varInit, varMin, varMax, prune, tau;

	bool detectShadows;
	uchar shadowVal;

	BinaryFunc cvtfunc;
};

