// MyCodeBook.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/legacy/legacy.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cxcore.h>

using namespace std;
using namespace cv;
#define  CHANNELS 3
//For connected components:
int CVCONTOUR_APPROX_LEVEL = 2;   // Approx.threshold - the bigger it is, the simpler is the boundary
int CVCLOSE_ITR = 1;				// How many iterations of erosion and/or dilation there should be
//#define CVPERIMSCALE 4			// image (width+height)/PERIMSCALE.  If contour length < this, delete that contour

//For learning background
const double COLORDIST_THREAD = 0.3;
//Just some convenience macros
#define CV_CVX_WHITE	CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK	CV_RGB(0x00,0x00,0x00)

typedef struct code_word
{
	float myRGB[CHANNELS];
	uchar learnHigh[CHANNELS];// High side threshold for learning  
	uchar learnLow[CHANNELS];// Low side threshold for learning  
	uchar max[CHANNELS]; //// High side of box boundary
	uchar min[CHANNELS];  //// Low side of box boundary
	int frequency;
	int t_last_update;  // This is book keeping to allow us to kill stale entries
	int stale;// max negative run (biggest period of inactivity)
} code_element;

typedef struct code_book
{
	code_element **cb;
	int numEntries;
	int t;
}codeBook;
///////////////////////////////////////////////////////////////////////////////////
// int updateCodeBook(uchar *p, codeBook &c, unsigned cbBounds)
// Updates the codebook entry with a new data point
//
// p			Pointer to a YUV pixel
// c			Codebook for this pixel
// cbBounds		Learning bounds for codebook (Rule of thumb: 10)
// numChannels	Number of color channels we're learning
double getcolordist(uchar* pRGBx, float* pRGBv)
{
	float xt = 0.0, vi = 0.0, xtvi = 0.0;
	for (int i = 0; i < CHANNELS; ++i)
	{
		xt += 1.0 * (*(pRGBx + i) / 255.0) * (*(pRGBx + i));
		vi += 1.0 * (*(pRGBv + i) / 255.0) * (*(pRGBv + i));
		xtvi += 1.0 * (*(pRGBx + i) / 255.0) * (*(pRGBv + i));
	}
	return std::pow(xt - xtvi * xtvi /  vi, 0.5);
}
int cvupdateCodeBook(uchar *p,uchar *pRGB, codeBook &c, unsigned *cbBounds, int numChannels = 3)
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
		for (int n = 0; n < numChannels; ++n)
		{
			//在这之间
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n]))
			{
				++matchChannel;
			}
		}
		if (matchChannel == numChannels || getcolordist(pRGB, c.cb[i]->myRGB) < COLORDIST_THREAD)
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
			c.cb[i]->learnHigh[n]--;
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

uchar cvbackgroundDiff(uchar *p,uchar* pRGB,  codeBook &c, int numChannels, int *minMod, int *maxMod)
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
		if (matchChannel == numChannels || getcolordist(pRGB, c.cb[i]->myRGB) < COLORDIST_THREAD)
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

int cvclearStaleEntries(codeBook &c)
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
int cvcountSegmentation(codeBook *c, Mat& I, Mat& raw, int numChannels, int *minMod, int *maxMod)
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
void cvconnectedComponents(Mat& mask, int poly1_hull0 = 1, float perimScale = 4.0, int *num = NULL, cv::Rect *bbs = NULL, cv::Point2i *centers = NULL)
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

void cvconnectedComponents(IplImage *mask, int poly1_hull0 = 1, float perimScale = 4.0 , int *num = NULL, CvRect *bbs = NULL, CvPoint *centers = NULL)
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

void cvconnectedComponents2(Mat& mask)
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
void cvconnectedComponents3(Mat& mask)
{
	morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(),cv::Point(0,0),1);
	erode(mask, mask, cv::Mat(), cv::Point(0,0), 1);
	medianBlur(mask, mask, 3);
	//dilate(mask, )
}
int _tmain(int argc, _TCHAR* argv[])
{
	Mat rawImage;
	Mat yuvImage;
	Mat maskCodeBook;
	codeBook* cb;
	unsigned cbBounds[CHANNELS];
	uchar* pRGB;
	uchar* pColor;
	int imageLen;
	int nChannels = CHANNELS;
	int minMod[CHANNELS];
	int maxMod[CHANNELS];
	cv::namedWindow("Raw");
	cv::namedWindow("CodeBook");
	VideoCapture oVideoInput;
	//"D://dataset//dataset//intermittentObjectMotion//streetLight//input//in%%06d.jpg" "D://dataset//results//intermittentObjectMotion//streetLight//"
	oVideoInput.open("D://dataset//dataset//cameraJitter//traffic//input//in%06d.jpg");
	if (!(oVideoInput.isOpened()))
	{
		fprintf(stderr, "can not open video file %s\n", argv[1]);
		return 0;
	}
	oVideoInput >> rawImage;
	oVideoInput.set(CV_CAP_PROP_POS_FRAMES,0);
	yuvImage.create(rawImage.size(), CV_8UC3);
	maskCodeBook.create(rawImage.size(), CV_8UC1);
	maskCodeBook = cv::Scalar_<uchar>(255);
	imageLen = rawImage.rows * rawImage.cols;
	cb = new codeBook[imageLen];
	for (int i = 0; i < imageLen; ++i)
	{
		cb[i].numEntries = 0;
	}
	for (int i = 0; i < nChannels; ++i)
	{
		cbBounds[i] = 10;
		minMod[i] = 20;
		maxMod[i] = 20;
	}
	int i = 1;
	while (TRUE)
	{
		oVideoInput>>rawImage;
		if (rawImage.empty())
		{
			break;
		}
		cvtColor(rawImage, yuvImage, CV_BGR2YCrCb);
		//
		//yuvImage = rawImage.clone();
		if (i <= 30)
		{
			pColor = (uchar*)yuvImage.data;
			pRGB = (uchar*)rawImage.data;
			for (int c = 0; c < imageLen; ++c)
			{
				cvupdateCodeBook(pColor,pRGB, cb[c], cbBounds, nChannels);
				pColor += 3;
				pRGB += 3;
			}
			if ( i == 30)
			{
				for (int c = 0; c < imageLen; ++c)
				{
					cvclearStaleEntries(cb[c]);
				}
			}
		}
		else
		{
			uchar maskPixelCodeBook;
			pColor = (uchar*)yuvImage.data;
			pRGB = (uchar*)rawImage.data;
			uchar* pMask = (uchar*)maskCodeBook.data;
			for (int c = 0; c < imageLen; ++c)
			{
				maskPixelCodeBook = cvbackgroundDiff(pColor, pRGB, cb[c], nChannels, minMod, maxMod);
				*pMask++ = maskPixelCodeBook;
				pColor += 3;
				pRGB += 3;
			}
			if (i % 30 == 0)
			{
				for (int c = 0; c < imageLen; ++c)
				{
					cvclearStaleEntries(cb[c]);
				}
			}
		}
		imshow("Raw", rawImage);
		imshow("code book pre", maskCodeBook);
		//IplImage* test = &maskCodeBook.operator IplImage();
		//cvconnectedComponents(test);
		cvconnectedComponents3(maskCodeBook);
		imshow("CodeBook", maskCodeBook);
		if (waitKey(30) == 27)
		{
			break;
		}
		i++;
	}
	destroyAllWindows();
	delete []cb;
	return 0;
}

