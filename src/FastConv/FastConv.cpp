
#include "FastConv.h"

CFastConv::CFastConv( void )
{
}

CFastConv::~CFastConv( void )
{
    reset();
}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/)
{


    return Error_t::kNoError;
}

Error_t CFastConv::reset()
{
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


    return Error_t::kNoError;
}
//TODO: [10] Implement the 'flushBuffer' function that will return the remaining result (reverb tail) after the last sample has been processed (end of input signal).
// flushBuffer is to be called once after the last process call. Question: How will you make it clear to the user what amount of memory to allocate for the output?
// Do you need an additional API function?

Error_t CFastConv::flushBuffer(float* pfOutputBuffer)
{
    return Error_t::kNoError;
}
