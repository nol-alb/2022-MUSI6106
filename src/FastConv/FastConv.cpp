
#include "FastConv.h"


CFastConv::CFastConv( void )
{
    this->reset();
}

CFastConv::~CFastConv( void )
{
    this->reset();
}

Error_t CFastConv::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/)
{
    if(eCompMode==kTimeDomain) {
        this->reset();
        //TODO: Set BIsInitialized
        m_pImpulseResponse = new float[iLengthOfIr];
        m_lengthofIR = iLengthOfIr;
        m_BlockLength = iBlockLength;
        for (int i = 0; i < iLengthOfIr; i++) {
            m_pImpulseResponse[i] = pfImpulseResponse[i];
        }
//    std::memcpy(m_pImpulseResponse, pfImpulseResponse, sizeof(float) * m_lengthofIR);
        m_pCRingBuffer = new CRingBuffer<float>(iLengthOfIr);
        type = eCompMode;
        bIsInitialized= true;
        return Error_t::kNoError;
    }
    else if(eCompMode==kFreqDomain)
    {
        this->reset();
        m_pImpulseResponse = new float[iLengthOfIr];
        m_lengthofIR = iLengthOfIr;
        m_BlockLength = iBlockLength;
        for (int i = 0; i < iLengthOfIr; i++) {
            m_pImpulseResponse[i] = pfImpulseResponse[i];
        }
        //Initialise the FFT
        type = eCompMode;
        CFft::createInstance(m_pCFft);
        m_pCFft->initInstance(2*m_BlockLength,1,CFft::kWindowHann,CFft::kNoWindow);

        //Calculate Number of Blocks in the Impulse Response

        numOfIRBlocks = std::max(static_cast<int>(std::ceil(m_lengthofIR / m_BlockLength)),1);

        //Create the IR Matrix to pre-calculate the freq domain representation
        ppFreqBlockedIR = new CFft::complex_t*[numOfIRBlocks];
        ppfRealBlockedIR = new float*[numOfIRBlocks];
        ppfImagBlockedIR = new float *[numOfIRBlocks];
        sBlockLength = m_BlockLength;
        ldbBlockLength = 2*m_BlockLength;
        ppfBlockedIR = new float* [numOfIRBlocks];
        for(int i=0;i<numOfIRBlocks;i++)
        {

            ppfBlockedIR[i] = new float[(2*m_BlockLength)];
            CVectorFloat::setZero(ppfBlockedIR[i],ldbBlockLength); //Zero padding so the size of each block is 2M
            CVectorFloat::copy(ppfBlockedIR[i],m_pImpulseResponse+(i*m_BlockLength),sBlockLength);
            ppFreqBlockedIR[i]=new CFft::complex_t[(2*m_BlockLength)];
            m_pCFft->doFft(ppFreqBlockedIR[i], ppfBlockedIR[i]);
            ppfRealBlockedIR[i] = new float[(m_BlockLength)];
            ppfImagBlockedIR[i]= new float[(m_BlockLength)];
            m_pCFft->splitRealImag(ppfRealBlockedIR[i],ppfImagBlockedIR[i],ppFreqBlockedIR[i]);
        }
        ppfProcessedOutputBlocks = new float*[numOfIRBlocks];
        for (int i=0; i<numOfIRBlocks;i++)
        {
            ppfProcessedOutputBlocks[i]=new float[(m_BlockLength)];
            CVectorFloat::setZero(ppfProcessedOutputBlocks[i],sBlockLength);
        }
        pfInputProcessing = new float[(2*m_BlockLength)];
        CVectorFloat::setZero(pfInputProcessing,ldbBlockLength);

        pfInvFFtProcessing = new float [(2*m_BlockLength)];
        pFFTProductProcess = new CFft::complex_t [(2*m_BlockLength)];
        pfProductImag = new float [(m_BlockLength)];
        pfProductReal = new float [(m_BlockLength)];
        pfRealInputProcessing = new float[(2*m_BlockLength)];
        pfImagInputProcessing = new float[(2*m_BlockLength)];
        pfreqInputProcessing = new CFft::complex_t [(2*m_BlockLength)];
        CVectorFloat::setZero(pfInputProcessing,ldbBlockLength);
        bIsInitialized= true;
        return Error_t::kNoError;
    }
}

Error_t CFastConv::reset()
{
    if(!bIsInitialized){
        return Error_t::kNotInitializedError;
    }
    else if(type==kTimeDomain) {
        delete[] m_pImpulseResponse;
        m_pImpulseResponse = 0;
        delete m_pCRingBuffer;
        m_pCRingBuffer = 0;
        bIsInitialized=false;


        return Error_t::kNoError;
    }
    else if(type==kFreqDomain)
    {
        delete[] m_pImpulseResponse;
        m_pImpulseResponse = 0;
        CFft::destroyInstance(m_pCFft);
        m_pCFft=0;
        for (int i=0; i<numOfIRBlocks;i++)
        {
            delete[] ppfBlockedIR[i];
            delete[] ppfImagBlockedIR[i];
            delete[] ppfProcessedOutputBlocks[i];
            delete[] ppFreqBlockedIR[i];
            delete[] ppfRealBlockedIR[i];
        }
        delete[] ppfBlockedIR;
        delete[] ppfImagBlockedIR;
        delete[] ppfProcessedOutputBlocks;
        delete[] ppFreqBlockedIR;
        delete[] ppfRealBlockedIR;
        ppfBlockedIR =nullptr;
        ppfImagBlockedIR=nullptr;
        ppfProcessedOutputBlocks=nullptr;
        ppFreqBlockedIR=nullptr;
        ppfRealBlockedIR=nullptr;

        delete[] pfInputProcessing;
        delete[] pfreqInputProcessing;
        delete[] pfInvFFtProcessing;
        delete[] pfProductImag;
        delete[] pfProductReal;
        delete[] pFFTProductProcess;
        delete[] pfRealInputProcessing;
        delete[] pfImagInputProcessing;

        pfInputProcessing=0;
        pFFTProductProcess=0;
        pfreqInputProcessing=0;
        pfInvFFtProcessing=0;
        pfProductImag=0;
        pfProductReal=0;
        pfRealInputProcessing=0;
        pfImagInputProcessing=0;
        pfInvFFtProcessing=0;
        bIsInitialized=false;

    }
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
    if(!bIsInitialized)
    {
        return Error_t::kNotInitializedError;
    }
    else if(type == kTimeDomain){
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
    CVectorFloat::setZero(pfOutputBuffer,iLengthOfBuffers);
    for (int i =0; i<iLengthOfBuffers; i++){
        m_pCRingBuffer->setReadIdx(m_pCRingBuffer->getWriteIdx()+1); //Ensures you multiply the first sample Input with the first sample of the flipped impulse response
        m_pCRingBuffer->putPostInc(pfInputBuffer[i]); //Move into the buffer and circularly shift for time convolution
        accum = 0;
        for(int j =m_lengthofIR-1; j>=0;j--){
            accum+= m_pImpulseResponse[j]* m_pCRingBuffer->getPostInc();
        }
        pfOutputBuffer[i] = accum;
    }
    return Error_t::kNoError;
}
Error_t CFastConv::freqdomainprocess(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers) {

    ///PseudoCode
    // outer loop iterates through the ilength of Buffers
    //Set a readBlockIDx=0 and set WriteBlockIdx = 0, for shift counters in the algorithm
    //We setup a 2M size input buffer and fill it from 0:M with zeros for block1 and M-2M with x1
    //We have a processed output matrix of size ypro[No of IR blocks][M], initialized with zeros
    //The output y[write] = ypro[ReadBlockIdx][write]  (Note: For first block M we will get zeros (Latency of the system))
    //Once the inputBuffer is filled to 2M we perform the convolution multiplication operation
    //Innerloop
    //We receive and processed output of ----

                                // 0 + x1*h1  <---- ReadIdx, WriteIdx
                                // 0 + x1*h2
                                // 0 + x1*h3
                                // 0 + x1*h4
    //Shift second half of the input to the left, to make room for new input Xprocess = [x1:x1]

    //Increment Write Idx, but before you do make sure the readIdx is equal to the write Idx
    //ReadIdx = 0, WriteIdx = 1
    //Repeat outer loop process
    //Have to setup the yprocessed[ReadIdx] = 0, to account for the last process being added to empty mem in the algorithm
    //After process the processed output is ----

                                                //0 +x2h4 <---ReadIdx
                                                //x1h2+x2h1 <---- WriteIdx
                                                //x1h3+x2h2
                                                //x1h4+x2h3
    //Shift the processed input again [x2:x2]
    //Increment the Write Idx
    //ReadIdx = 1, WriteIdx = 2
    // Now
            //0 +x2h4
            //x1h2+x2h1 <---ReadIdx
            //x1h3+x2h2 <---- WriteIdx
            //x1h4+x2h3
    for (int i=0; i<iLengthOfBuffers; i++)
    {
        //TODO: ADD ASSERTS FOR LESS BUFFERSIZE COMPARED TO BLOCKlENGTH
        //Set the second half of the inputBuffer with the input process values
        //PointofWrite sets the new input block x1,x2 from center of pfInputBuffer
        pfInputProcessing[PointOfWrite+m_BlockLength] = pfInputBuffer[i];
        //TheBlockNoReading, helps us control the shift in blocks while overlap
        pfOutputBuffer[i] = ppfProcessedOutputBlocks[BlockNoReading][PointOfWrite];
        PointOfWrite++;
        if(PointOfWrite==m_BlockLength)
        {
            for (int j=0;j<m_BlockLength;j++)
            {
                ppfProcessedOutputBlocks[BlockNoReading][j]=0;
            }
            PointOfWrite=0;
            m_pCFft->doFft(pfreqInputProcessing,pfInputProcessing);
            m_pCFft->splitRealImag(pfRealInputProcessing,pfImagInputProcessing,pfreqInputProcessing);
            for (int j = 0; j<numOfIRBlocks; j++)
            {
                complexMultiply(pfRealInputProcessing,pfImagInputProcessing,ppfRealBlockedIR[j],ppfImagBlockedIR[j],pfProductReal,pfProductImag,m_lengthofIR);
                m_pCFft->mergeRealImag(pFFTProductProcess,pfProductReal,pfProductImag);
                m_pCFft->doInvFft(pfInvFFtProcessing,pFFTProductProcess);
                int process_writeIdx = (BlockNoWriting + j) % numOfIRBlocks; //A temporary variable to help write the convolution sum with each block of the IR
                for (int k =0; k<m_BlockLength;k++)
                {
                    ppfProcessedOutputBlocks[process_writeIdx][k] += pfInvFFtProcessing[k+m_BlockLength];
                }
            }
            BlockNoReading=BlockNoWriting;
            //Shift the current input block to the left to make room for the new x2
            for (int k = 0; k<m_BlockLength;k++)
            {
                pfInputProcessing[k] = pfInputProcessing[k+m_BlockLength];
            }
            //Make the 2 dimensional processblocks behave like a ringbuffer (Can just use the ringbuffer to control these shifts)

            BlockNoWriting=(BlockNoWriting+1)%numOfIRBlocks;
        }

    }




    return Error_t::kNoError;
}

//FUNCTION TO MAKE COMPLEX MULTIPLICAION, GIVEN COMPLEX_T
//splitRealImag

Error_t CFastConv::complexMultiply(float* realInput1, float* imagInput1, float* realInput2, float* imagInput2, float* realOutput, float* imagOutput, int outputLength ) {
    for (int i = 0; i < outputLength; i++) {
        realOutput[i] = (realInput1[i] * realInput2[i]) - (imagInput1[i] * imagInput2[i]) * 2 * m_BlockLength;
        imagOutput[i] = (realInput2[i] * imagInput1[i]) + (realInput1[i] * imagInput2[i]) * 2 * m_BlockLength;
    }
    return Error_t::kNoError;
}


//TODO: [10] Implement the 'flushBuffer' function that will return the remaining result (reverb tail) after the last sample has been processed (end of input signal).
// flushBuffer is to be called once after the last process call. Question: How will you make it clear to the user what amount of memory to allocate for the output?
// Do you need an additional API function?

Error_t CFastConv::flushBuffer(float* pfOutputBuffer)
{
    if(type == kTimeDomain)
    {
        float *pfInputFlush = new float[m_lengthofIR - 1];
        long long int sbufferlength = m_lengthofIR-1;
        CVectorFloat::setZero(pfInputFlush, sbufferlength);
        process(pfOutputBuffer, pfInputFlush, m_lengthofIR - 1);
        delete[] pfInputFlush;
    }
    if(type == kFreqDomain)
    {
        float *pfInputFlush = new float[m_BlockLength + m_lengthofIR - 1]; //Think about the initial delay of the zeros of blocksize
        long long int sbufferlength = m_lengthofIR + m_BlockLength-1;
        CVectorFloat::setZero(pfInputFlush, sbufferlength);
        process(pfOutputBuffer, pfInputFlush, m_BlockLength+m_lengthofIR - 1);
        delete[] pfInputFlush;

    }

//    float accum = 0;
//    for(int i=0; i<m_lengthofIR-1;i++){
//        m_pCRingBuffer->setReadIdx(m_pCRingBuffer->getWriteIdx()+1);
//        m_pCRingBuffer->putPostInc(0);
//        accum = 0;
//        for(int j =m_lengthofIR-1; j>=0;j--){
//            accum+= m_pImpulseResponse[j]* m_pCRingBuffer->getPostInc();
//        }
//        pfOutputBuffer[i] = accum;
//    }
    return Error_t::kNoError;
}

