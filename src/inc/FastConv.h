
#if !defined(__FastConv_HEADER_INCLUDED__)
#define __FastConv_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Fft.h"
#include "Vector.h"

/*! \brief interface for fast convolution
*/
class CFastConv
{
public:
    enum ConvCompMode_t
    {
        kTimeDomain,
        kFreqDomain,

        kNumConvCompModes
    };

    CFastConv(void);
    virtual ~CFastConv(void);

    /*! initializes the class with the impulse response and the block length
    \param pfImpulseResponse impulse response samples (mono only)
    \param iLengthOfIr length of impulse response
    \param iBlockLength processing block size
    \return Error_t
    */
    Error_t init(float* pfImpulseResponse, int iLengthOfIr, int iBlockLength = 8192, ConvCompMode_t eCompMode = kFreqDomain);
    //If initialised for mode set a flag, use flag to call the process block of time or frequency domain

    /*! resets all internal class members
    \return Error_t
    */
    Error_t reset ();

    /*! computes the output with reverb
    \param pfOutputBuffer (mono)
    \param pfInputBuffer (mono)
    \param iLengthOfBuffers can be anything from 1 sample to 10000000 samples
    \return Error_t
    */
    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);
    // https://www.youtube.com/watch?v=fYggIQTaVx4

    /*! return the 'tail' after processing has finished (identical to feeding in zeros
    \param pfOutputBuffer (mono)
    \return Error_t
    */
    Error_t flushBuffer(float* pfOutputBuffer);
    // https://dsp.stackexchange.com/questions/74710/convolution-reverb-calculation

private:
    CRingBuffer<float>  *m_pCRingBuffer;
    float *m_pImpulseResponse;
    int m_lengthofIR;
    int m_BlockLength;
    ConvCompMode_t type;
    Error_t timedomainprocess(float *pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);
    Error_t freqdomainprocess(float *pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);
    //Required pointers for fft handlers
    CFft* m_pCFft = nullptr;

    Error_t complexMultiply(float* realInput, float* imagInput, float* realInput2, float* imagInput2, float* realOutput, float* imagOutput, int outputLength);



};


#endif
