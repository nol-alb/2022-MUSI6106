
#if !defined(__FastConv_HEADER_INCLUDED__)
#define __FastConv_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Fft.h"
#include "Vector.h"

/*! \brief interface for fast convolution
*/
class CConvbase;

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
    CRingBuffer<float>  *m_pCRingBuffer=0;
    bool bIsInitialized;
    float *m_pImpulseResponse=0;
    int m_lengthofIR=0;
    int m_BlockLength=0;
    ConvCompMode_t type;
    Error_t timedomainprocess(float *pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);
    Error_t freqdomainprocess(float *pfOutputBuffer, const float* pfInputBuffer, int iLengthOfBuffers);
    //Required pointers for fft handlers
    CFft* m_pCFft = nullptr;
    float** ppfBlockedIR=0;
    CFft::complex_t** ppFreqBlockedIR = nullptr;
    float ** ppfRealBlockedIR = nullptr;
    float ** ppfImagBlockedIR = nullptr;
    int numOfIRBlocks=0;
    long long ldbBlockLength=0;
    long long sBlockLength=0;
    int PointOfWrite = 0;
    int PointOfRead = 0;
    int BlockNoWriting = 0;
    int BlockNoReading=0;
    float* pfInputProcessing=nullptr;
    float** ppfProcessedOutputBlocks = nullptr;
    CFft::complex_t* pfreqInputProcessing = nullptr;
    CFft::complex_t* pFFTProductProcess = nullptr;
    float* pfRealInputProcessing = nullptr;
    float* pfImagInputProcessing = nullptr;
    float* pfProductReal = nullptr;
    float* pfProductImag = nullptr;
    float* pfInvFFtProcessing = nullptr;

    Error_t complexMultiply(float* realInput, float* imagInput, float* realInput2, float* imagInput2, float* realOutput, float* imagOutput, int outputLength);

    CConvbase* m_pCConv = 0;


};


#endif
