
#include "FastConv.h"


CFastConv::CFastConv( void ):m_pCRingBuffer(0), m_pImpulseResponse(0),
m_lengthofIR(0),
m_BlockLength(0)
{
    this->reset();
}

CFastConv::~CFastConv( void )
{
    reset();
}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/)
{
    this->reset();
    //TODO: Set BIsInitialized
    m_pImpulseResponse = new float[iLengthOfIr];
    m_lengthofIR = iLengthOfIr;
    m_BlockLength = iBlockLength;
    for (int i=0; i < iLengthOfIr;i++){
        m_pImpulseResponse[i] = pfImpulseResponse[i];
    }
//    std::memcpy(m_pImpulseResponse, pfImpulseResponse, sizeof(float) * m_lengthofIR);
    m_pCRingBuffer = new CRingBuffer<float>(iLengthOfIr);
    type = eCompMode;
    return Error_t::kNoError;
}

Error_t CFastConv::reset()
{
    delete[] m_pImpulseResponse;
    m_pImpulseResponse = 0;
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
    float accum;
    for (int i =0; i<iLengthOfBuffers; i++){
        m_pCRingBuffer->setReadIdx(m_pCRingBuffer->getWriteIdx()+1); //Ensures you multiply the first sample Input with the first sample of the flipped impulse response
        m_pCRingBuffer->putPostInc(pfInputBuffer[i]);
        accum = 0;
        for(int j =m_lengthofIR-1; j>=0;j--){
            accum+= m_pImpulseResponse[j]* m_pCRingBuffer->getPostInc();
        }
        pfOutputBuffer[i] = accum;
    }
    return Error_t::kNoError;
}
Error_t CFastConv::freqdomainprocess(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers) {
    //Initialise the FFT
    CFft::createInstance(m_pCFft);
    m_pCFft->initInstance(2*m_BlockLength,1,CFft::kWindowHann,CFft::kNoWindow);

    //Calculate Number of Blocks in the Impulse Response
    int numOfIRBlocks = 0;
    // CHECK ROUMDOMG
    numOfIRBlocks = static_cast<int>(m_lengthofIR / m_BlockLength);

    //Create the IR Matrix to pre-calculate the freq domain representation
    float** ppfBlockedIR = new float* [numOfIRBlocks];
    CFft::complex_t** ppFreqBlockedIR = nullptr;
    float ** ppfRealBlockedIR = nullptr;
    float ** ppfImagBlockedIR = nullptr;
    ppFreqBlockedIR = new CFft::complex_t*[numOfIRBlocks];
    ppfRealBlockedIR = new float*[numOfIRBlocks];
    ppfImagBlockedIR = new float *[numOfIRBlocks];
    long long ldbBlockLength;
    long long sBlockLength;
    sBlockLength = m_BlockLength;
    ldbBlockLength = 2*m_BlockLength;


    // Calculate the freq domain representation of the Impulse Response and split into real and imaginary
    for(int i=0;i<numOfIRBlocks;i++)
    {
        ppfBlockedIR[i] = new float[(2*m_BlockLength)];
        CVectorFloat::setZero(ppfBlockedIR[i],ldbBlockLength); //Zero padding so the size of each block is 2M
        CVectorFloat::copy(ppfBlockedIR[i],m_pImpulseResponse+(i*m_BlockLength),sBlockLength);
        ppFreqBlockedIR[i]=new CFft::complex_t[(2*m_BlockLength)];
        m_pCFft->doFft(ppFreqBlockedIR[i], ppfBlockedIR[i]);
        ppfImagBlockedIR[i] = new float[(2*m_BlockLength)];
        ppfImagBlockedIR[i]= new float[(2*m_BlockLength)];
        m_pCFft->splitRealImag(ppfRealBlockedIR[i],ppfImagBlockedIR[i],ppFreqBlockedIR[i]);
    }
    int PointOfWrite,PointOfRead = 0;
    int BlockNoWriting, BlockNoReading=0;
    float* pfInputProcessing=nullptr;
    float** ppfProcessedOutputBlocks = new float*[numOfIRBlocks];
    for (int i=0; i<numOfIRBlocks;i++)
    {
        ppfProcessedOutputBlocks[i]=new float[(m_BlockLength)];
        CVectorFloat::setZero(ppfProcessedOutputBlocks[i],sBlockLength);
    }
    pfInputProcessing = new float[(2*m_BlockLength)];
    CFft::complex_t* pfreqInputProcessing = nullptr;
    float* pfRealInputProcessing = nullptr;
    float* pfImagInputProcessing = nullptr;
    float* pfProductReal = nullptr;
    float* pfProductImag = nullptr;
    pfProductImag = new float [(2*m_BlockLength)];
    pfProductReal = new float [(2*m_BlockLength)];
    pfRealInputProcessing = new float[(2*m_BlockLength)];
    pfImagInputProcessing = new float[(2*m_BlockLength)];
    pfreqInputProcessing = new float[(2*m_BlockLength)];
    CVectorFloat::setZero(pfInputProcessing,ldbBlockLength);

    for (int i=0; i<iLengthOfBuffers; i++)
    {
        //Set the second half of the inputBuffer with the input process values
        pfInputProcessing[PointOfWrite+m_BlockLength] = pfInputBuffer[i];
        pfOutputBuffer[i] = ppfProcessedOutputBlocks[BlockNoReading][PointOfWrite];
        PointOfWrite++;
        if(PointOfWrite==m_BlockLength)
        {
            PointOfWrite=0;
            m_pCFft->doFft(pfreqInputProcessing,pfInputProcessing);
            m_pCFft->splitRealImag(pfRealInputProcessing,pfImagInputProcessing,pfreqInputProcessing);
            for (int j = 0; j<numOfIRBlocks; j++)
            {
                complexMultiply(pfRealInputProcessing,pfImagInputProcessing,ppfRealBlockedIR[j],ppfImagBlockedIR[j],pfProductReal,pfProductImag,m_lengthofIR);
                
                // merge

                // inv fft

            }
            

        }

    }



    return Error_t::kNoError;
}

//FUNCTION TO MAKE COMPLEX MULTIPLICAION, GIVEN COMPLEX_T
//splitRealImag

Error_t CFastConv::complexMultiply(float* realInput1, float* imagInput1, float* realInput2, float* imagInput2, float* realOutput, float* imagOutput, int outputLength ) {
    for (int i = 0; i < outputLength; i++) {
        realOutput[i] = (realInput1[i] * realInput2[i]) - (imagInput1[i] * imagInput2[i]);
        imagOutput[i] = (realInput2[i] * imagInput1[i]) + (realInput1[i] * imagInput2[i]);
    }
    return Error_t::kNoError;
}


//TODO: [10] Implement the 'flushBuffer' function that will return the remaining result (reverb tail) after the last sample has been processed (end of input signal).
// flushBuffer is to be called once after the last process call. Question: How will you make it clear to the user what amount of memory to allocate for the output?
// Do you need an additional API function?

Error_t CFastConv::flushBuffer(float* pfOutputBuffer)
{
    float accum = 0;
    for(int i=0; i<m_lengthofIR-1;i++){
        m_pCRingBuffer->setReadIdx(m_pCRingBuffer->getWriteIdx()+1);
        m_pCRingBuffer->putPostInc(0);
        accum = 0;
        for(int j =m_lengthofIR-1; j>=0;j--){
            accum+= m_pImpulseResponse[j]* m_pCRingBuffer->getPostInc();
        }
        pfOutputBuffer[i] = accum;
    }
    return Error_t::kNoError;
}
