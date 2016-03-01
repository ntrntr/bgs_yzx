
#include "stdafx.h"
#include "TBackground.h"

TBackground::TBackground(void)
{
  std::cout << "TBackground()" << std::endl;
}

TBackground::~TBackground(void)
{
  Clear();
  std::cout << "~TBackground()" << std::endl;
}

void TBackground::Clear(void)
{
}

void TBackground::Reset(void)
{
}

int TBackground::GetParameterCount(void)
{
  return 0;
}

std::string TBackground::GetParameterName(int nInd)
{
  return "";
}

std::string TBackground::GetParameterValue(int nInd)
{
  return "";
}

int TBackground::SetParameterValue(int nInd, std::string csNew)
{
  return 0;
}

int TBackground::Init(IplImage * pSource)
{
  return 0;
}

bool TBackground::isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  bool bResult = true;

  if(pSource == NULL || pSource->nChannels != 1 || pSource->depth != IPL_DEPTH_8U)
    bResult = false;

  if(bResult)
  {
    int nbl, nbc;
    nbl = pSource->height;
    nbc = pSource->width;
    
    if(pBackground == NULL || pBackground->width != nbc || pBackground->height != nbl || pBackground->imageSize != pSource->imageSize)
      bResult = false;
    
    if(pMotionMask == NULL || pMotionMask->width != nbc || pMotionMask->height != nbl || pMotionMask->imageSize != pSource->imageSize)
      bResult = false;
  }

  return bResult;
}

int TBackground::UpdateBackground(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  return 0;
}

IplImage *TBackground::CreateTestImg()
{
  IplImage *pImage = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 3);

  if(pImage != NULL)
    cvSetZero(pImage);

  return pImage;
}

int TBackground::UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd)
{
  int nErr = 0;
  CvScalar Color;
  unsigned char *ptr;

  if(pTest == NULL || !isInitOk(pSource, pBackground, pSource))
    nErr = 1;

  if(!nErr)
  {
    if(pTest->width != 256 || pTest->height != 256 || pTest->nChannels != 3)
      nErr = 1;

    if(nX < 0 || nX > pSource->width || nY < 0 || nY > pSource->height)
      nErr = 1;

    switch(nInd)
    {
      case 0 : Color = cvScalar(128, 0, 0); break;
      case 1 : Color = cvScalar(0, 128, 0); break;
      case 2 : Color = cvScalar(0, 0, 128); break;
      default : nErr = 1;
    }
  }

  if(!nErr)
  {
    int l, c;
    ptr = (unsigned char *)(pTest->imageData);
    c = *ptr;
    
    cvLine(pTest, cvPoint(c, 0), cvPoint(c, 255), cvScalar(0));
    *ptr += 1;

    ptr = (unsigned char *)(pBackground->imageData + pBackground->widthStep * nY);
    ptr += nX;
    l = *ptr;

    cvLine(pTest, cvPoint(c, l - 5), cvPoint(c, l + 5), Color);

    ptr = (unsigned char *)(pSource->imageData + pSource->widthStep * nY);
    ptr += nX;
    l = *ptr;

    ptr = (unsigned char *)(pTest->imageData + pTest->widthStep * l);
    ptr += (c * 3) + nInd;
    *ptr = 255;
  }

  return nErr;
}
