
#include "FastConv.h"

CFastConv::CFastConv( void ):m_pCRingBuffer(0), m_pImpulseResponse(0),
m_lengthofIR(0),
m_IBlockLength(0)
{
}

CFastConv::~CFastConv( void )
{
    reset();
}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/)
{
    m_pImpulseResponse = new float[iLengthOfIr];
    m_lengthofIR = iLengthOfIr;
    m_pImpulseResponse = pfImpulseResponse;
    m_pCRingBuffer = new CRingBuffer<float>(iLengthOfIr);
    type = eCompMode;
    return Error_t::kNoError;
}

Error_t CFastConv::reset()
{
//    delete m_pImpulseResponse;
//    m_pImpulseResponse = 0;
    delete m_pCRingBuffer;
    m_pCRingBuffer = 0;

    return Error_t::kNoError;
}
//TODO: [10] Declare the necessary class members and implement a time domain convolution (simple time domain FIR filter)
// in FastConv.cpp. This code should be executed if the flag kTimeDomain is set in init.
// ote the variable input block size and remember that the output length of the process function has to equal exactly the number of input samples
// under any circumstances for this and all other items in this assignment.

//TODO: [50] Implement (uniformly) partitioned fast convolution: see this book excerptActions  (5.3.2, pp 161-167,
// pay special attention to figure 5.58) for an explanation. It might be best if you first implement the blocking of the IR with time domain convolution to verify buffering and overlap and add,
// and only then replace the time domain convolution with the frequency domain multiplication. Note that the buffering will add latency to the output.
// Also note that the FFT scaling,while consistent between transform and inverse transform will need scaling with the FFT length.
// Utilize the FFT functions 'splitRealImag' and 'mergeRealImag' so you don't have to worry about the internal FFT format.

Error_t CFastConv::process (float* pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers )
{
    if(type == kTimeDomain){
        CFastConv::timedomainprocess(pfOutputBuffer, pfInputBuffer, iLengthOfBuffers);
    }
    else if(type ==kFreqDomain){
        CFastConv::freqdomainprocess(pfOutputBuffer,pfInputBuffer,iLengthOfBuffers);
    }
    else{
        return Error_t::kFunctionIllegalCallError;
    }
    return Error_t::kNoError;
}

Error_t CFastConv::timedomainprocess(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers) {

    for (int i =0; i<iLengthOfBuffers; i++){
        m_pCRingBuffer->setReadIdx(m_pCRingBuffer->getWriteIdx()+1);
        m_pCRingBuffer->putPostInc(pfInputBuffer[i]);
        float accum = 0;
        for(int j =m_lengthofIR-1; j>=0;j--){
            accum+= m_pImpulseResponse[j]* m_pCRingBuffer->getPostInc();
        }
        pfOutputBuffer[i] = accum;
    }
    return Error_t::kNoError;
}
Error_t CFastConv::freqdomainprocess(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers) {
    return Error_t::kNoError;
}
//TODO: [10] Implement the 'flushBuffer' function that will return the remaining result (reverb tail) after the last sample has been processed (end of input signal).
// flushBuffer is to be called once after the last process call. Question: How will you make it clear to the user what amount of memory to allocate for the output?
// Do you need an additional API function?

Error_t CFastConv::flushBuffer(float* pfOutputBuffer)
{
    return Error_t::kNoError;
}
