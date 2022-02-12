//
//  CombFilter.h
//  MUSI6106
//
//  Created by Noel  Alben on 2/7/22.
//

#ifndef CombFilter_h
#define CombFilter_h
#include "ErrorDef.h"
#include "RingBuffer.h"


// Base class definition for CombFilter FIR AND IIR (Brief definitions keep the implementation hidden)
class CCombFilterBase
{
public:
    CCombFilterBase(int DelayLengthIns, int iNumChannels, float gain=0.2);
    //You don't need to instantiate an object to call this function, make destructor with virtual
    // Make all the implementations hidden, make a seperate cpp file in CombFilter Source to run the functions
    virtual ~CCombFilterBase();
    
    Error_t resetComb();
    Error_t setGain(float gain);
    float getGain();
    Error_t setDelay(int DelayLengthinSamples);
    int getDelay();
    virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)=0;
    CRingBuffer<float>  **m_RingBuffptr;


    // process needs to be called withuot an object, make virtual

//Add members only accessed in the class definition or member classes ex:FIRFILTER,IIRFILTER
protected:
    int     m_iNumChannels;
    float m_gain;
    int m_DelayLengthIns;
    
};

//Making a child class from CCombFilterBase for FIR
class CCombFilterFir : public CCombFilterBase
{
    //    //make constructor and destructor
public:
    CCombFilterFir (int DelayLengthIns, int iNumChannels,float gain=0.2):CCombFilterBase(DelayLengthIns, iNumChannels,gain){};
    virtual ~CCombFilterFir (){};

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};

/*! \brief IIR comb filter implementation
*/
////Similar child classs for IIR filter
class CCombFilterIir : public CCombFilterBase
{
public:
    CCombFilterIir (int DelayLengthIns, int iNumChannels,float gain=0.2):CCombFilterBase(DelayLengthIns, iNumChannels,gain){};
    virtual ~CCombFilterIir (){};

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};

#endif /* CombFilter_h */
