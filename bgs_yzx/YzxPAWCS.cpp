#include "stdafx.h"
#include "YzxPAWCS.h"


CYzxPAWCS::CYzxPAWCS() :m_nR_x(20), m_nLBSPThresholdOffset(2), m_nSamplesForMovingAvgs(500)
{
	
}


CYzxPAWCS::~CYzxPAWCS()
{
}

void CYzxPAWCS::init(const Mat& oInitImg, const Mat& oRoi)
{

}

void CYzxPAWCS::refreshModel(int nBaseOccCount, float fOccDecrFrac, bool bForceFGUpdate /*= false*/)
{

}

void CYzxPAWCS::operator()(InputArray image, OutputArray fgmask, double learningRateOverride /*= 0*/)
{

}

void CYzxPAWCS::getBackgroundImage(OutputArray backgroundImage) const
{

}

void CYzxPAWCS::getBackgroundDescriptorsImage(OutputArray backgroundDescImage) const
{

}

void CYzxPAWCS::CleanupDictionaries()
{

}

float CYzxPAWCS::GetLocalWordWeight(const LocalWordBase* w, size_t nCurFrame, size_t nOffset)
{

}

float CYzxPAWCS::GetGlobalWordWeight(const GlobalWordBase* w)
{

}
