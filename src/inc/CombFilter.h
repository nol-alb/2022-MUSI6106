//
//  CombFilter.h
//  MUSI6106
//
//  Created by Noel  Alben on 2/7/22.
//

#ifndef CombFilter_h
#define CombFilter_h
#include "ErrorDef.h"
// Base class definition for CombFilter FIR AND IIR (Brief definitions keep the implementation hidden)
class CCombFilterBase
{
public:
    CCombFilterBase(int iMaxDelayLengthInFrames, int iNumChannels);
    //You don't need to instantiate an object to call this function, make destructor with virtual
    // Make all the implementations hidden, make a seperate cpp file in CombFilter Source to run the functions
    virtual ~CCombFilterBase();
    // use :: to use the enums from CCombFilterIf
    Error_t setParam (CCombFilterIf::FilterParam_t eParam, float fParamValue);
    float   getParam (CCombFilterIf::FilterParam_t eParam) const;
    // process needs to be called withuot an object, make virtual
    virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
//Add members only accessed in the class definition or member classes ex:FIRFILTER,IIRFILTER
protected:
    CRingBuffer<float>  **m_ppCRingBuffer;
    int     m_iNumChannels;
};
//Making a child class from CCombFilterBase for FIR

class CCombFilterFIR : public CCombFilterBase
{
    //make constructor and destructor
public:
    CCombFilterFIR(int iMaxDelayLengthInFrames, int iNumChannels):CCombFilterBase(iMaxDelayLengthInFrames, iNumChannels) {};
    virtual ~CCombFilterFIR();
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};
//Similar child classs for IIR filter
class CCombFilterIIR : public CCombFilterBase
{
public:
    CCombFilterIIR (int iMaxDelayLengthInFrames, int iNumChannels);
    virtual ~CCombFilterIIR (){};

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};

#endif /* CombFilter_h */
