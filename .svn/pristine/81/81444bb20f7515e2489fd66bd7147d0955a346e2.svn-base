#include "stdafx.h"
#include "MyAdpativeMOG.h"


using namespace cv;

CMyAdpativeMOG::CMyAdpativeMOG(void)
{
	frameSize = cv::Size(0, 0);
	frameType = 0;

	nframes = 0;
	nmixtures = defaultNMixtures;
	history = defaultHistory;
	varThreshold = defaultVarThreshold;
	backgroundRatio = defaultBackgroundRatio;
	noiseSigma = defaultNoiseSigma;
}

CMyAdpativeMOG::CMyAdpativeMOG(int _history, int _nmixtures, double _backgroundRatio, double _noiseSigma)
{
	frameSize = Size(0, 0);
	frameType = 0;
	nframes = 0;
	nmixtures = min(_nmixtures > 0 ? _nmixtures : defaultNMixtures, 8);
	history = _history > 0 ? _history : defaultHistory;
	varThreshold = defaultVarThreshold;
	backgroundRatio = min(_backgroundRatio > 0 ? _backgroundRatio : 0.95, 1.0);
	noiseSigma = _noiseSigma <= 0 ? defaultNoiseSigma : _noiseSigma;
}


CMyAdpativeMOG::~CMyAdpativeMOG(void)
{
}

void CMyAdpativeMOG::operator()(cv::InputArray _image, cv::OutputArray _fgmask, double learningRate)
{
	Mat image = _image.getMat();
	bool needToInitialize = nframes == 0 || learningRate >= 1 || image.size() != frameSize || image.type() != frameType;

	if (needToInitialize)
	{
		initialize(image.size(), image.type());
	}
	CV_Assert(image.depth() == CV_8U);
	_fgmask.create(image.size(), CV_8UC1);
	Mat fgmask = _fgmask.getMat();

	++nframes;
	learningRate = learningRate >= 0 && nframes > 1 ? learningRate : 1. / min(nframes, history);
	CV_Assert(learningRate >= 0);
	if (image.type() == CV_8UC1)
	{
		process8uC1(image, fgmask, learningRate, bgmodel, nmixtures, backgroundRatio, varThreshold, noiseSigma);
	}
	else if (image.type() == CV_8UC3)
	{
		process8uC3(image, fgmask, learningRate, bgmodel, nmixtures, backgroundRatio, varThreshold, noiseSigma);
	}
	else
	{
		CV_Error(CV_StsUnsupportedFormat, "Only 1- and 3-channel 8-bit images are supported in BackgroundSubtractorMOG");
	}
}

void CMyAdpativeMOG::getBackgroundImage(cv::Mat& image) const
{
	image.create(frameSize, frameType);
	image = cv::Scalar::all(0);
	int channels = image.channels();
	MixData<Vec3f>* mptr = (MixData<Vec3f>*)bgmodel.data;
	int rows = image.rows, cols = image.cols;
	int K = nmixtures;
	for (int y = 0; y < rows; ++y)
	{
		uchar* dst = image.ptr<uchar>(y);
		for (int x = 0; x < cols; ++x, mptr += K, dst += channels)
		{
			Vec3f sum(0, 0, 0);
			for (int k = 0; k < K; ++k)
			{
				sum += mptr[k].weight * mptr[k].mean;
			}
			for (int c = 0; c < channels; ++c)
			{
				dst[c] = (uchar)sum[c];
			}
		}
	}
}

void CMyAdpativeMOG::initialize(cv::Size _frameSize, int _frameType)
{
	frameSize = _frameSize;
	frameType = _frameType;
	nframes = 0;
	int nchannels = CV_MAT_CN(frameType);
	bgmodel.create(1, frameSize.height * frameSize.width * nmixtures * (2 + 2 * nchannels), CV_32F);
	bgmodel = Scalar::all(0);
}

void CMyAdpativeMOG::process8uC1(const cv::Mat& image, cv::Mat& fgmask, double learningRate, cv::Mat& bgmodel, int nmixtures, double backgroundRatio, double varThreshold, double noiseSigma)
{
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = static_cast<float>(learningRate), T = static_cast<float>(backgroundRatio), vT = static_cast<float>(varThreshold);
	int K = nmixtures;
	const float w0 = (float)defaultInitialWeight;
	const float sk0 = (float)(w0 / (defaultNoiseSigma * 2 * sqrt(3.)));
	const float var0 = (float)(defaultNoiseSigma * defaultNoiseSigma * 4);
	const float minVar = (float)(noiseSigma * noiseSigma);
	//1 1
	// 111
	// 111
	MixData<float>* mptr = (MixData<float>*)bgmodel.data;
	for (y = 0; y < rows; ++y)
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);
		if (alpha > 0)
		{
			for (x = 0; x < cols; ++x, mptr += K)
			{
				float wsum = 0;
				//RGB
				float pix = src[x];
				int kHit = -1, kForeground = -1;
				for (k = 0; k < K; ++k)
				{
					float w = mptr[k].weight;
					wsum += w;
					if (w < FLT_EPSILON)
					{
						break;
					}
					float mu = mptr[k].mean;
					float var = mptr[k].var;
					float diff = pix - mu;
					float d2 = diff * diff;
					//matched
					if (d2 < vT * var)
					{
						//w_k,t = w_k,t-1 + alpha(M_k,t - w_k,t-1); 
						wsum -= w;
						float dw = alpha*(1.f - w);
						mptr[k].weight = w + dw;
						//u_t = (1 - p)u_t-1 + p * X_t;
						mptr[k].mean = mu + alpha * diff;
						var = max(var + alpha*(d2 - var), minVar);
						mptr[k].var = var;
						mptr[k].sortKey = w / sqrt(var);

						for (k1 = k - 1; k1 >= 0; --k1)
						{
							if (mptr[k1].sortKey >= mptr[k1 + 1].sortKey)
							{
								break;
							}
							std::swap(mptr[k1], mptr[k1 + 1]);
						}
						kHit = k1 + 1;
						break;
					}
				}
				//no match
				if (kHit < 0)
				{
					kHit = k = min(k, K - 1);
					wsum += w0 - mptr[k].weight;
					mptr[k].weight = w0;
					mptr[k].mean = pix;
					mptr[k].var = var0;
					mptr[k].sortKey = sk0;
				}
				else
				{
					for (; k < K; ++k)
					{
						wsum += mptr[k].weight;
					}
				}

				float wscale = 1.f / wsum;
				wsum = 0;
				for (k = 0; k < K; ++k)
				{
					wsum += mptr[k].weight *= wscale;
					mptr[k].sortKey *= wscale;
					if (wsum > T && kForeground < 0)
					{
						kForeground = k + 1;
					}

				}
				dst[x] = (uchar)(-(kHit >= kForeground));

			}
		}
		else
		{
			for (x = 0; x < cols; ++x, mptr += K)
			{

				//RGB
				float pix = src[x];
				int kHit = -1, kForeground = -1;
				for (k = 0; k < K; ++k)
				{
					if (mptr[k].weight < FLT_EPSILON)
					{
						break;
					}
					float mu = mptr[k].mean;
					float var = mptr[k].var;
					float diff = pix - mu;
					float d2 = diff * diff;
					//matched
					if (d2 < vT * var)
					{
						kHit = k;
						break;
					}
				}
				//no match
				if (kHit >= 0)
				{
					float wsum = 0;
					for (k = 0; k < K; ++k)
					{
						wsum += mptr[k].weight;
						if (wsum > T)
						{
							kForeground = k + 1;
							break;
						}
					}
				}
				dst[x] = (uchar)(kHit < 0 || (kHit >= kForeground ? 255 : 0));
			}
		}
	}
}

void CMyAdpativeMOG::process8uC3(const cv::Mat& image, cv::Mat& fgmask, double learningRate, cv::Mat& bgmodel, int nmixtures, double backgroundRatio, double varThreshold, double noiseSigma)
{
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = static_cast<float>(learningRate), T = static_cast<float>(backgroundRatio), vT = static_cast<float>(varThreshold);
	int K = nmixtures;
	const float w0 = (float)defaultInitialWeight;
	const float sk0 = (float)(w0 / (defaultNoiseSigma * 2 * sqrt(3.)));
	const float var0 = (float)(defaultNoiseSigma * defaultNoiseSigma * 4);
	const float minVar = (float)(noiseSigma * noiseSigma);
	//1 1
	// 111
	// 111
	MixData<Vec3f>* mptr = (MixData<Vec3f>*)bgmodel.data;
	for (y = 0; y < rows; ++y)
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);
		if (alpha > 0)
		{
			for (x = 0; x < cols; ++x, mptr += K)
			{
				float wsum = 0;
				//RGB
				Vec3f pix(src[x * 3], src[x * 3 + 1], src[x * 3 + 2]);
				int kHit = -1, kForeground = -1;
				for (k = 0; k < K; ++k)
				{
					float w = mptr[k].weight;
					wsum += w;
					if (w < FLT_EPSILON)
					{
						break;
					}
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					//Gray = R*0.299 + G*0.587 + B*0.114
					uchar x1minusx2 = static_cast<uchar>(abs((mu[0] * 0.114 + mu[1] * 0.587 + mu[2] * 0.299) - (pix[0] * 0.114 + pix[1] * 0.587 + pix[2] * 0.299)));
					//matched
					if (d2 < vT * (var[0] + var[1] + var[2]))
					{
						//w_k,t = w_k,t-1 + alpha(M_k,t - w_k,t-1); 
						wsum -= w;
						float alpha_use_no_linear;
						if (x1minusx2 <= 1)
						{
							alpha_use_no_linear = alpha;
						}
						else
						{
							alpha_use_no_linear = pow(alpha, x1minusx2);
						}
						
						float dw = alpha_use_no_linear*(1.f - w);
						mptr[k].weight = w + dw;
						//u_t = (1 - p)u_t-1 + p * X_t;
						mptr[k].mean = mu + alpha_use_no_linear * diff;
						var = Vec3f(
							max(var[0] + alpha_use_no_linear*(diff[0] * diff[0] - var[0]), minVar),
							max(var[1] + alpha_use_no_linear*(diff[1] * diff[1] - var[1]), minVar),
							max(var[2] + alpha_use_no_linear*(diff[2] * diff[2] - var[2]), minVar)
							);
						mptr[k].var = var;
						mptr[k].sortKey = w / sqrt(var[0] + var[1] + var[2]);

						for (k1 = k - 1; k1 >= 0; --k1)
						{
							if (mptr[k1].sortKey >= mptr[k1 + 1].sortKey)
							{
								break;
							}
							std::swap(mptr[k1], mptr[k1 + 1]);
						}
						kHit = k1 + 1;
						break;
					}
				}
				//no match
				if (kHit < 0)
				{
					kHit = k = min(k, K - 1);
					wsum += w0 - mptr[k].weight;
					mptr[k].weight = w0;
					mptr[k].mean = pix;
					mptr[k].var = Vec3f(var0, var0, var0);
					mptr[k].sortKey = sk0;
				}
				else
				{
					for (; k < K; ++k)
					{
						wsum += mptr[k].weight;
					}
				}

				float wscale = 1.f / wsum;
				wsum = 0;
				for (k = 0; k < K; ++k)
				{
					wsum += mptr[k].weight *= wscale;
					mptr[k].sortKey *= wscale;
					if (wsum > T && kForeground < 0)
					{
						kForeground = k + 1;
					}

				}
				dst[x] = (uchar)(-(kHit >= kForeground));

			}
		}
		else
		{
			for (x = 0; x < cols; ++x, mptr += K)
			{
				//RGB
				Vec3f pix(src[x * 3], src[x * 3 + 1], src[x * 3 + 2]);
				int kHit = -1, kForeground = -1;
				for (k = 0; k < K; ++k)
				{
					if (mptr[k].weight < FLT_EPSILON)
					{
						break;
					}
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					//matched
					if (d2 < vT * (var[0] + var[1] + var[2]))
					{
						kHit = k;
						break;
					}
				}
				//no match
				if (kHit >= 0)
				{
					float wsum = 0;
					for (k = 0; k < K; ++k)
					{
						wsum += mptr[k].weight;
						if (wsum > T)
						{
							kForeground = k + 1;
							break;
						}
					}
				}
				dst[x] = (uchar)(kHit < 0 || (kHit >= kForeground ? 255 : 0));
			}
		}
	}
}


void CMyAdpativeMOG::modifyMask(cv::Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(0, 0), 1);
	erode(mask, mask, cv::Mat(), cv::Point(0, 0), 1);
	medianBlur(mask, mask, 3);
}


void CMyAdpativeMOG::saveImage(cv::Mat& image, std::string& outpath, std::string& filename, int count)
{
	char str[30];
	sprintf(str, filename.c_str(), count);
	string tmp = outpath + str;
	cv::imwrite(tmp.c_str(), image);
}