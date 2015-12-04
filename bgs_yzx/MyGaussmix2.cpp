#include "stdafx.h"
#include "MyGaussmix2.h"


MyGaussmix2::MyGaussmix2(void)
{
	frameSize = Size(0, 0);
	frameType = 0;
	nframes = 0;
	history = defaultHistory2;
	varThreshold = defaultVarThreshold2;
	bShadowDetection = 1;

	nmixtures = defaultNMixtures2;
	backgroundRatio = defaultBackgroundRatio2;
	fVarInit = defaultVarInit2;
	fVarMax = defaultVarMax2;
	fVarMin = defaultVarMin2;
	varThresholdGen = defaultVarThresholdGen2;
	fCT = defaultfCT2;
	nShadowDetection = defaultnShadowDetection2;
	fTau = defaultfTau;
}

MyGaussmix2::MyGaussmix2(int _history, float _varThreshold, bool _bShadowDetection)
{
	frameSize = Size(0,0);
	frameType = 0;

	nframes = 0;
	history = _history > 0 ? _history : defaultHistory2;
	varThreshold = (_varThreshold>0)? _varThreshold : defaultVarThreshold2;
	bShadowDetection = _bShadowDetection;

	nmixtures = defaultNMixtures2;
	backgroundRatio = defaultBackgroundRatio2;
	fVarInit = defaultVarInit2;
	fVarMax  = defaultVarMax2;
	fVarMin = defaultVarMin2;

	varThresholdGen = defaultVarThresholdGen2;
	fCT = defaultfCT2;
	nShadowDetection =  defaultnShadowDetection2;
	fTau = defaultfTau;
}


MyGaussmix2::~MyGaussmix2(void)
{
}

CV_INLINE bool MyGaussmix2::detectShadowGMM(const float* data, int nchannels, int nmodes, const GMM* gmm, const float* mean, float Tb, float TB, float tau)
{
	float tWeight = 0;
	for (int mode = 0; mode < nmodes; ++mode, mean += nchannels)
	{
		GMM g = gmm[mode];
		float numerator = 0.0f;
		float denominator = 0.0f;
		for (int c = 0;  c < nchannels; ++c)
		{
			numerator += data[c] * mean[c];
			denominator += mean[c] * mean[c];
		}
		if (denominator == 0)
		{
			return false;
		}
		// if tau < a < 1 then also check the color distortion
		if (numerator <= denominator && numerator >= tau * denominator)
		{
			float a = numerator / denominator;
			float dist2a = 0.0f;
			for (int c = 0; c < nchannels; ++c)
			{
				float dD = a * mean[c] - data[c];
				dist2a += dD * dD;
			}
			if ( dist2a < Tb * g.variance*a*a)
			{
				return true;
			}
		}
		tWeight += g.weight;
		if (tWeight > TB)
		{
			return false;
		}
	}
	return false;
}

void MyGaussmix2::initialize(Size _frameSize, int _frameType)
{
	frameSize = _frameSize;
	frameType = _frameType;
	int nchannels = CV_MAT_CN(frameType);
	CV_Assert(nchannels <= CV_CN_MAX);
	bgmodel.create(1, frameSize.height * frameSize.width * nmixtures * (2 + nchannels), CV_32F);
	bgmodelUsedModes.create(frameSize, CV_8U);
	bgmodelUsedModes = Scalar::all(0);
}

void MyGaussmix2::operator()(InputArray _image, OutputArray _fgmask, double learningRate)
{
	Mat image = _image.getMat();
	bool needToInitialize = nframes == 0 || learningRate >= 1 || image.size() != frameSize || image.type() != frameType;
	if (needToInitialize)
	{
		initialize(image.size(), image.type());
	}
	_fgmask.create(image.size(), CV_8U);
	Mat fgmask = _fgmask.getMat();
	++nframes;
	learningRate = learningRate >= 0 && nframes > 1 ? learningRate : 1./ min(2 * nframes, history);

	CV_Assert(learningRate >= 0);
	parallel_for_(Range(0, image.rows), 
		MOG2Invoker(image,
		fgmask, 
		(GMM*)bgmodel.data, 
		(float*)(bgmodel.data + sizeof(GMM) * nmixtures * image.rows * image.cols),
		bgmodelUsedModes.data,
		nmixtures,
		(float)learningRate, 
		(float)varThreshold, 
		backgroundRatio, 
		varThresholdGen, 
		fVarInit, 
		fVarMin, 
		fVarMax, 
		float(-learningRate * fCT), 
		fTau, 
		bShadowDetection, 
		nShadowDetection));
}

void MyGaussmix2::getBackgroundImage(OutputArray backgroundImage) const
{
	int nchannels = CV_MAT_CN(frameType);
	CV_Assert(nchannels == 3);
	Mat meanBackground(frameSize, CV_8UC3, Scalar::all(0));
	int firstGaussianIdx = 0;
	const GMM* gmm = (GMM*)bgmodel.data;
	const Vec3f* mean = reinterpret_cast<const Vec3f*>(gmm + frameSize.width * frameSize.height * nmixtures);

	for (int row = 0; row < meanBackground.rows; ++row)
	{
		for (int col = 0; col < meanBackground.cols; ++col)
		{
			int nmodels = bgmodelUsedModes.at<uchar>(row, col);
			Vec3f meanVal;
			float totalWeight = 0.f;
			for (int gaussianIdx = firstGaussianIdx; gaussianIdx < firstGaussianIdx + nmodels; ++gaussianIdx)
			{
				GMM gaussian = gmm[gaussianIdx];
				meanVal += gaussian.weight * mean[gaussianIdx];
				totalWeight += gaussian.weight;
				if (totalWeight > backgroundRatio)
				{
					break;
				}
			}
			meanVal *= (1.f / totalWeight);
			meanBackground.at<Vec3b>(row, col) = Vec3b(meanVal);
			firstGaussianIdx += nmixtures;
		}
	}
	switch (CV_MAT_CN(frameType))
	{
	case 1:
		{
		vector<Mat> channels;
		split(meanBackground, channels);
		channels[0].copyTo(backgroundImage);
		break;
		}
		
	case 3:
		{
		meanBackground.copyTo(backgroundImage);
		break;
		}
		
	default:
		CV_Error(CV_StsUnsupportedFormat, "");
		break;
	}
}

void MyGaussmix2::saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count)
{
	char str[30];
	sprintf_s(str, filename.c_str(), count);
	string tmp = outpath + str;
	cv::imwrite(tmp.c_str(), image);
}
