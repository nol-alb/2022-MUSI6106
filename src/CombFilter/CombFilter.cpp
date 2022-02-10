//
//  CombFilter.cpp
//  MUSI6106
//
//  Created by Noel  Alben on 2/7/22.
//
// standard headers
#include <limits>

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"
#include "RingBuffer.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

CCombFilterBase::CCombFilterBase(int delayLengthInS, int iNumChannels, float gain): m_Buffptr(0)
{
    m_Buffptr = new CRingBuffer<float>*[iNumChannels];
    for (int i; i<iNumChannels;i++)
    {
        m_Buffptr[i] = new CRingBuffer<float>(delayLengthInS);
        m_Buffptr[i]->reset();
        
    }
    
   
    
    
}



CCombFilterBase::~CCombFilterBase()
{
    for (int i; i<m_iNumChannels;i++)
    {
        m_Buffptr[i]->~CRingBuffer();
    }
}

Error_t CCombFilterBase::setGain(float gain)
{
    m_gain = gain;
}
float CCombFilterBase::getGain()
{
    return m_gain;
}
Error_t CCombFilterBase::setDelay(int DelayLengthinSamples)
{
    m_DelayLengthIns=DelayLengthinSamples;
    
}
int CCombFilterBase::getDelay()
{
    return m_DelayLengthIns;
    
}


Error_t CCombFilterBase::resetComb()
{
    m_DelayLengthIns=0;
    m_gain=0;
    
    return Error_t::kNoError;
}
//Error_t CCombFilterBase::setParam(CCombFilterIf::FilterParam_t eParam, float fParamValue)
//{
//    return Error_t::kNoError;
//
//}
//Error_t CCombFilterBase::getParam(CCombFilterIf::FilterParam_t eParam)
//{
//    return Error_t::kNoError;
//}

Error_t CCombFilterFir::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    return Error_t::kNoError;
}
Error_t CCombFilterIir::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    return Error_t::kNoError;
    
}
