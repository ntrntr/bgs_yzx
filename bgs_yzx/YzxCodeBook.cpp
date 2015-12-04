
#include "stdafx.h"
#include "YzxCodeBook.h"

YzxCodeBook::YzxCodeBook(void):CVCONTOUR_APPROX_LEVEL(2),CVCLOSE_ITR(1),COLORDIST_THREAD(0.0)
{
}


YzxCodeBook::~YzxCodeBook(void)
{
}

double YzxCodeBook::getcolordist(uchar* pRGBx, float* pRGBv)
{
	double xt = 0.0, vi = 0.0, xtvi = 0.0;
	for (int i = 0; i < CHANNELS; ++i)
	{
		xt += 1.0 * (*(pRGBx + i) / 255.0) * (*(pRGBx + i));
		vi += 1.0 * (*(pRGBv + i) / 255.0) * (*(pRGBv + i));
		xtvi += 1.0 * (*(pRGBx + i) / 255.0) * (*(pRGBv + i));
	}
	return std::pow(xt - xtvi * xtvi /  vi, 0.5);
}
int YzxCodeBook::cvupdateCodeBook(uchar *p,uchar *pRGB, codeBook &c, unsigned *cbBounds, int numChannels)
{
	if (c.numEntries == 0) c.t = 0;
	c.t += 1;

	int n;
	unsigned int high[3];
	unsigned int low[3];
	//get min and max
	for (n = 0; n < numChannels; ++n)
	{
		high[n] = *(p + n) + *(cbBounds + n);
		if (high[n] > 255)
		{
			high[n] = 255;
		}

		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0)
		{
			low[n] = 0;
		}
	}
	int matchChannel, i;
	//if match update
	for (i = 0; i < c.numEntries; ++i)
	{
		matchChannel = 0;
		for (n = 0; n < numChannels; ++n)
		{
			//在这之间
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n]))
			{
				++matchChannel;
			}
		}
		if (matchChannel == numChannels /*|| getcolordist(pRGB, c.cb[i]->myRGB) < COLORDIST_THREAD*/)
		{
			//更新这个codeword
			c.cb[i]->t_last_update = c.t;

			for (n = 0; n < numChannels; ++n)
			{
				c.cb[i]->max[n] = MAX(c.cb[i]->max[n], *(p + n));
				c.cb[i]->min[n] = MIN(c.cb[i]->min[n], *(p + n));
				c.cb[i]->myRGB[n] = ((c.cb[i]->frequency * c.cb[i]->myRGB[n] + (*(pRGB+n)))/255.0)/(1.0+c.cb[i]->frequency);
			}
			c.cb[i]->frequency++;
			//exit circle
			break;
		}
	}
	//去除太久没用的codeword
	for (int myi = 0; myi < c.numEntries; ++myi)
	{
		int negRun = c.t - c.cb[myi]->t_last_update;
		if (c.cb[myi]->stale < negRun)
		{
			c.cb[myi]->stale = negRun;
		}
	}
	if (i == c.numEntries)
	{
		//create new
		code_element **foo = new code_element *[c.numEntries + 1];
		for (int myi = 0; myi < c.numEntries; ++myi)
		{
			foo[myi] = c.cb[myi];
		}
		foo[c.numEntries] = new code_element;
		if (c.numEntries)
		{
			delete[] c.cb;
		}
		c.cb = foo;
		for (n = 0; n < numChannels; ++n)
		{
			c.cb[c.numEntries]->learnHigh[n] = high[n];
			c.cb[c.numEntries]->learnLow[n] = low[n];
			c.cb[c.numEntries]->max[n] = *(p + n);
			c.cb[c.numEntries]->min[n] = *(p + n);
			c.cb[c.numEntries]->myRGB[n] = *(pRGB + n) * 1.0 / 255;
		}
		c.cb[c.numEntries]->t_last_update = c.t;
		c.cb[c.numEntries]->stale = 0;
		c.cb[c.numEntries]->frequency = 1;
		c.numEntries ++;
	}

	//slow adjust learning bounds 
	for (n = 0; n < numChannels; ++n)
	{
		if (c.cb[i]->learnHigh[n] < high[n]) 
		{
			c.cb[i]->learnHigh[n]++;
		}
		if (c.cb[i]->learnLow[n] > low[n]) 
		{
			c.cb[i]->learnLow[n]--;
		}
	}
	return i;
}

///////////////////////////////////////////////////////////////////////////////////
// uchar cvbackgroundDiff(uchar *p, codeBook &c, int minMod, int maxMod)
// Given a pixel and a code book, determine if the pixel is covered by the codebook
//
// p		pixel pointer (YUV interleaved)
// c		codebook reference
// numChannels  Number of channels we are testing
// maxMod	Add this (possibly negative) number onto max level when code_element determining if new pixel is foreground
// minMod	Subract this (possible negative) number from min level code_element when determining if pixel is foreground

uchar YzxCodeBook::cvbackgroundDiff(uchar *p,uchar* pRGB,  codeBook &c, int numChannels, int *minMod, int *maxMod)
{
	int matchChannel;
	int i;
	for (i = 0; i < c.numEntries; ++i)
	{
		matchChannel = 0;
		for (int n = 0; n < numChannels; ++n)
		{
			//在这之间
			if ((c.cb[i]->min[n]- minMod[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->max[n] + maxMod[n]))
			{
				++matchChannel;
			}
			else
			{
				break;
			}
		}
		if (matchChannel == numChannels /*|| getcolordist(pRGB, c.cb[i]->myRGB) < COLORDIST_THREAD*/)
		{
			//found;
			break;
		}
	}
	if (i >= c.numEntries)
	{
		return 255;
	}
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//int clearStaleEntries(codeBook &c)
// After you've learned for some period of time, periodically call this to clear out stale codebook entries
//
//c		Codebook to clean up
//

int YzxCodeBook::cvclearStaleEntries(codeBook &c)
{
	//T/2
	int staleThresh = c.t >> 1;
	int *keep = new int[c.numEntries];
	int keepCnt = 0;
	for (int i = 0; i < c.numEntries; ++i)
	{
		if (c.cb[i]->stale > staleThresh)
		{
			keep[i] = 0;
		}
		else
		{
			keep[i] = 1;
			++keepCnt;
		}
	}
	c.t = 0;
	code_element **foo = new code_element *[keepCnt];
	int k = 0;
	for (int ii = 0; ii < c.numEntries; ++ii)
	{
		if (keep[ii])
		{
			foo[k] = c.cb[ii];
			foo[k]->stale = 0;
			foo[k]->t_last_update = 0;
			++k;
		}
	}
	delete []keep;
	delete []c.cb;
	c.cb = foo;
	int numCleared = c.numEntries - keepCnt;
	c.numEntries = keepCnt;
	return (numCleared);
}

/////////////////////////////////////////////////////////////////////////////////
//int countSegmentation(codeBook *c, IplImage *I)
//
//Count how many pixels are detected as foreground
// c	Codebook
// I	Image (yuv, 24 bits)
// numChannels  Number of channels we are testing
// maxMod	Add this (possibly negative) number onto max level when code_element determining if new pixel is foreground
// minMod	Subtract this (possible negative) number from min level code_element when determining if pixel is foreground
//
// NOTES: 
// minMod and maxMod must have length numChannels, e.g. 3 channels => minMod[3], maxMod[3].
//
//Return
// Count of fg pixels
//
int YzxCodeBook::cvcountSegmentation(codeBook *c, Mat& I, Mat& raw, int numChannels, int *minMod, int *maxMod)
{
	int count = 0;
	int i;
	uchar* pColor;
	uchar* pRGB;
	int imageLen = I.rows  * I.cols;
	pColor = (uchar*)I.data;
	pRGB = (uchar*)raw.data;
	for (i = 0; i < imageLen; ++i)
	{
		if (cvbackgroundDiff(pColor,pRGB,  c[i], numChannels, minMod, maxMod))
		{
			count++;
		}
		pColor += 3;
	}
	return count;
}

///////////////////////////////////////////////////////////////////////////////////////////
//void cvconnectedComponents(IplImage *mask, int poly1_hull0, float perimScale, int *num, CvRect *bbs, CvPoint *centers)
// This cleans up the forground segmentation mask derived from calls to cvbackgroundDiff
//
// mask			Is a grayscale (8 bit depth) "raw" mask image which will be cleaned up
//
// OPTIONAL PARAMETERS:
// poly1_hull0	If set, approximate connected component by (DEFAULT) polygon, or else convex hull (0)
// perimScale 	Len = image (width+height)/perimScale.  If contour len < this, delete that contour (DEFAULT: 4)
// num			Maximum number of rectangles and/or centers to return, on return, will contain number filled (DEFAULT: NULL)
// bbs			Pointer to bounding box rectangle vector of length num.  (DEFAULT SETTING: NULL)
// centers		Pointer to contour centers vectore of length num (DEFULT: NULL)
//
void YzxCodeBook::cvconnectedComponents(Mat& mask, int poly1_hull0, float perimScale, int *num, cv::Rect *bbs, cv::Point2i *centers)
{
	static CvMemStorage* mem_storage = NULL;
	static CvSeq* contours = NULL;
	//static Seq contours;
	morphologyEx(mask, mask, CV_MOP_OPEN, NULL, cv::Point(-1,-1), CVCLOSE_ITR);
	morphologyEx(mask, mask, CV_MOP_CLOSE, NULL, cv::Point(-1,-1), CVCLOSE_ITR);
	//morphologyEx(mask, mask, NULL, NULL, CV_MOP_OPEN, CVCLOSE_ITR);
	if (mem_storage == NULL) mem_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(mem_storage);
	CvContourScanner scanner = cvStartFindContours((CvArr*)&mask, mem_storage, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
	while ((c = cvFindNextContour(scanner)) != NULL)
	{
		double len = cvContourPerimeter(c);
		double q = (mask.rows + mask.cols) / perimScale;
		if (len < q)
		{
			cvSubstituteContour(scanner, NULL);
		}
		else
		{
			CvSeq* c_new;
			if (poly1_hull0)
			{
				c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL, 0);
			}
			else
			{
				c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);
			}
			cvSubstituteContour(scanner, c_new);
			numCont++;
		}
	}
	contours = cvEndFindContours(&scanner);
	//mask.zeros();
	//Zero(mask);
	//cvZero(mask);
	cvSet((CvArr*)&mask, cvScalarAll(0), 0);
	Mat maskTmp;
	if (num != NULL)
	{
		int N = *num, numFilled = 0, i = 0;
		CvMoments moments;
		double M00, M01, M10;
		maskTmp = mask.clone();
		for (i = 0, c = contours; c != NULL; c = c->h_next, ++i)
		{
			if (i < N)
			{
				cvDrawContours((CvArr*)&maskTmp, c, CV_CVX_WHITE, CV_CVX_WHITE,  -1, CV_FILLED, 8);
				if (centers != NULL)
				{
					cvMoments((CvArr*)&maskTmp, &moments, 1);
					M00 = cvGetSpatialMoment(&moments, 0, 0);
					M10 = cvGetSpatialMoment(&moments, 1, 0);
					M01 = cvGetSpatialMoment(&moments, 0, 1);
					centers[i].x = (int)(M10 / M00);
					centers[i].y = (int)(M01 / M00);
				}
				if (bbs != NULL)
				{
					bbs[i] = cvBoundingRect(c);
				}
				cvSet((CvArr*)&mask, cvScalarAll(0), 0);
				numFilled++;
			}
			cvDrawContours((CvArr*)&mask, c, CV_CVX_WHITE, CV_CVX_WHITE, -1, CV_FILLED, 8);

		}
		*num = numFilled;
	}
	else
	{
		for( c=contours; c != NULL; c = c->h_next )
		{
			cvDrawContours((CvArr*)&mask,c,CV_CVX_WHITE, CV_CVX_BLACK,-1,CV_FILLED,8);
		}
	}

}

void YzxCodeBook::cvconnectedComponents(IplImage *mask, int poly1_hull0, float perimScale , int *num, CvRect *bbs, CvPoint *centers)
{
	static CvMemStorage*	mem_storage	= NULL;
	static CvSeq*			contours	= NULL;
	//CLEAN UP RAW MASK
	cvMorphologyEx( mask, mask, NULL, NULL, CV_MOP_OPEN, CVCLOSE_ITR );
	cvMorphologyEx( mask, mask, NULL, NULL, CV_MOP_CLOSE, CVCLOSE_ITR );

	//FIND CONTOURS AROUND ONLY BIGGER REGIONS
	if( mem_storage==NULL ) mem_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(mem_storage);

	CvContourScanner scanner = cvStartFindContours(mask,mem_storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
	while( (c = cvFindNextContour( scanner )) != NULL )
	{
		double len = cvContourPerimeter( c );
		double q = (mask->height + mask->width) /perimScale;   //calculate perimeter len threshold
		if( len < q ) //Get rid of blob if it's perimeter is too small
		{
			cvSubstituteContour( scanner, NULL );
		}
		else //Smooth it's edges if it's large enough
		{
			CvSeq* c_new;
			if(poly1_hull0) //Polygonal approximation of the segmentation
				c_new = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL,0);
			else //Convex Hull of the segmentation
				c_new = cvConvexHull2(c,mem_storage,CV_CLOCKWISE,1);
			cvSubstituteContour( scanner, c_new );
			numCont++;
		}
	}
	contours = cvEndFindContours( &scanner );

	// PAINT THE FOUND REGIONS BACK INTO THE IMAGE
	cvZero( mask );
	IplImage *maskTemp;
	//CALC CENTER OF MASS AND OR BOUNDING RECTANGLES
	if(num != NULL)
	{
		int N = *num, numFilled = 0, i=0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);
		for(i=0, c=contours; c != NULL; c = c->h_next,i++ )
		{
			if(i < N) //Only process up to *num of them
			{
				cvDrawContours(maskTemp,c,CV_CVX_WHITE, CV_CVX_WHITE,-1,CV_FILLED,8);
				//Find the center of each contour
				if(centers != NULL)
				{
					cvMoments(maskTemp,&moments,1);
					M00 = cvGetSpatialMoment(&moments,0,0);
					M10 = cvGetSpatialMoment(&moments,1,0);
					M01 = cvGetSpatialMoment(&moments,0,1);
					centers[i].x = (int)(M10/M00);
					centers[i].y = (int)(M01/M00);
				}
				//Bounding rectangles around blobs
				if(bbs != NULL)
				{
					bbs[i] = cvBoundingRect(c);
				}
				cvZero(maskTemp);
				numFilled++;
			}
			//Draw filled contours into mask
			cvDrawContours(mask,c,CV_CVX_WHITE,CV_CVX_WHITE,-1,CV_FILLED,8); //draw to central mask
		} //end looping over contours
		*num = numFilled;
		cvReleaseImage( &maskTemp);
	}
	//ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
	else
	{
		for( c=contours; c != NULL; c = c->h_next )
		{
			cvDrawContours(mask,c,CV_CVX_WHITE, CV_CVX_BLACK,-1,CV_FILLED,8);
		}
	}
}

void YzxCodeBook::cvconnectedComponents2(Mat& mask)
{
	Mat preflood, floodedholes;
	morphologyEx(mask, preflood, cv::MORPH_CLOSE, cv::Mat());
	preflood.copyTo(floodedholes);
	cv::floodFill(floodedholes, cv::Point(0,0), UCHAR_MAX);
	bitwise_not(floodedholes, floodedholes);
	erode(preflood, preflood, cv::Mat(), cv::Point(0,0), 3);
	bitwise_or(mask, floodedholes, mask);
	bitwise_or(mask, preflood, mask);
	//medianBlur(mask, mask, 3);
	//dilate(mask, )
}
void YzxCodeBook::cvconnectedComponents3(Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(),cv::Point(0,0),1);
	erode(mask, mask, cv::Mat(), cv::Point(0,0), 1);
	medianBlur(mask, mask, 3);
	//dilate(mask, )
}


void YzxCodeBook::modifyMask(cv::Mat& oCurrFGMask)
{
	cv::bitwise_xor(oCurrFGMask,m_oLastRawFGMask,m_oCurrRawFGBlinkMask);
	cv::bitwise_or(m_oCurrRawFGBlinkMask,m_oLastRawFGBlinkMask,m_oBlinksFrame);
	m_oCurrRawFGBlinkMask.copyTo(m_oLastRawFGBlinkMask);
	oCurrFGMask.copyTo(m_oLastRawFGMask);
	cv::morphologyEx(oCurrFGMask,m_oFGMask_PreFlood,cv::MORPH_CLOSE,cv::Mat());
	m_oFGMask_PreFlood.copyTo(m_oFGMask_FloodedHoles);
	cv::floodFill(m_oFGMask_FloodedHoles,cv::Point(0,0),UCHAR_MAX);
	cv::bitwise_not(m_oFGMask_FloodedHoles,m_oFGMask_FloodedHoles);
	cv::erode(m_oFGMask_PreFlood,m_oFGMask_PreFlood,cv::Mat(),cv::Point(-1,-1),3);
	cv::bitwise_or(oCurrFGMask,m_oFGMask_FloodedHoles,oCurrFGMask);
	cv::bitwise_or(oCurrFGMask,m_oFGMask_PreFlood,oCurrFGMask);
	cv::medianBlur(oCurrFGMask,m_oLastFGMask,3);
	cv::dilate(m_oLastFGMask,m_oLastFGMask_dilated,cv::Mat(),cv::Point(-1,-1),3);
	cv::bitwise_and(m_oBlinksFrame,m_oLastFGMask_dilated_inverted,m_oBlinksFrame);
	cv::bitwise_not(m_oLastFGMask_dilated,m_oLastFGMask_dilated_inverted);
	cv::bitwise_and(m_oBlinksFrame,m_oLastFGMask_dilated_inverted,m_oBlinksFrame);
	m_oLastFGMask.copyTo(oCurrFGMask);
}


void YzxCodeBook::initialize(const cv::Mat& oInitImg)
{
	m_oImgSize = oInitImg.size();
	m_oBlinksFrame.create(m_oImgSize,CV_8UC1);
	m_oBlinksFrame = cv::Scalar_<uchar>(0);
	m_oLastRawFGMask.create(m_oImgSize,CV_8UC1);
	m_oLastRawFGMask = cv::Scalar_<uchar>(0);
	m_oLastRawFGBlinkMask.create(m_oImgSize,CV_8UC1);
	m_oLastRawFGBlinkMask = cv::Scalar_<uchar>(0);
	m_oLastFGMask_dilated_inverted.create(m_oImgSize,CV_8UC1);
	m_oLastFGMask_dilated_inverted = cv::Scalar_<uchar>(0);
}
