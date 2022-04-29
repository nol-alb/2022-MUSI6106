//
// Created by Noel  Alben on 4/28/22.
//


#include "CConvbase.h"
#include "Vector.h"
#include "Util.h"

CConvbase::CConvbase() {
}
CConvbase::~CConvbase() {
}

Error_t CConvbase::reset()
{
    m_iIrLength =0;
    m_iBlockLength = 0;
    m_bIsInitialized = false;

    return Error_t::kNoError;

}

Error_t CConvTime::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength)
{
    if (iLengthOfIr<= 0)
        return Error_t::kFunctionInvalidArgsError;
    if(!pfImpulseResponse)
        return Error_t::kFunctionInvalidArgsError;
    this->reset();

    m_iIrLength = iLengthOfIr;
    m_iBlockLength = iBlockLength;
    long long int l_iLenghtOfIR;
    l_iLenghtOfIR = m_iIrLength;


    m_pCRingBuff = new CRingBuffer<float>(m_iIrLength);
    m_pfImpulseResponse = new float[m_iIrLength];
    CVectorFloat::copy(m_pfImpulseResponse, pfImpulseResponse,l_iLenghtOfIR);

    m_bIsInitialized = true;
    return Error_t::kNoError;

}

Error_t CConvTime::reset()
{
    delete[] m_pfImpulseResponse;
    m_pfImpulseResponse = 0;
    delete m_pCRingBuff;
    m_pCRingBuff = 0;

    return Error_t::kNoError;

}
Error_t CConvTime::process(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthOfBuffers)
{

    if (!m_bIsInitialized)
        return Error_t::kFunctionIllegalCallError;

    if (!pfInputBuffer)
        return Error_t::kFunctionInvalidArgsError;

    if (!pfOutputBuffer)
        return Error_t::kFunctionInvalidArgsError;

    if (iLengthOfBuffers <= 0)
        return Error_t::kFunctionInvalidArgsError;

        CVectorFloat::setZero(pfOutputBuffer,static_cast<long long int>(iLengthOfBuffers));
        for (int i = 0; i < iLengthOfBuffers; i++)
        {
            //Add input to the ring buffer in circular manner
            m_pCRingBuff->putPostInc(pfInputBuffer[i]);
            float acc = 0;
            for (int j =0; j < m_iIrLength; j++)
            {
                //fill the accumulator and read the ring buffer backwards for the flip
                acc += m_pfImpulseResponse[j] * m_pCRingBuff->get(-j);
            }
            pfOutputBuffer[i]=acc;
            m_pCRingBuff->getPostInc();
            assert(m_pCRingBuff->getReadIdx() == m_pCRingBuff->getWriteIdx());
        }
    return Error_t::kNoError;
    }

Error_t CConvTime::flushBuffer(float* pfOutputBuffer)
{
    float* pfFlushInputBuffer = new float[m_iIrLength- 1];
    CVectorFloat::setZero(pfOutputBuffer, m_iIrLength-1);
    process(pfOutputBuffer, pfFlushInputBuffer, m_iIrLength - 1);
    delete[] pfFlushInputBuffer;

    return Error_t::kNoError;
}

Error_t CConvFFT::init(float *pfImpulseResponse, int iLengthOfIr, int iBlockLength)
{
    if(!CUtil::isPowOf2(iBlockLength) || iBlockLength<=0)
        return Error_t::kFunctionInvalidArgsError;
    if (iLengthOfIr<=0)
        return Error_t::kFunctionInvalidArgsError;
    if(!pfImpulseResponse)
        return Error_t::kFunctionInvalidArgsError;
    this->reset();


    m_pfImpulseResponse=pfImpulseResponse;
    m_iBlockLength=iBlockLength;
    m_iFftLength = iBlockLength << 1;
    m_iIrLength = iLengthOfIr;
    m_iIRBlockNum = static_cast<int>(std::ceil(static_cast<float>(m_iIrLength) / static_cast<float>(m_iBlockLength)));
    CFft::createInstance(m_pcFFT);
    m_pcFFT->initInstance(m_iBlockLength, 2, CFft::kWindowHann,CFft::kNoWindow);
    pfFFTRealTemp = new float[m_iBlockLength + 1];
    pfFFTImagTemp = new float[m_iBlockLength + 1];
    pfCurrentBlockFFTReal = new float[m_iBlockLength + 1];
    pfCurrentBlockFFTImag = new float[m_iBlockLength + 1];
    pfIRTemp = new float[2*m_iBlockLength];
    pfIFFTTemp= new float [2*m_iBlockLength];
    long long int ldbBlockLength = 2*m_iBlockLength;
    CVectorFloat::setZero(pfIRTemp,ldbBlockLength);

    pfComplexTemp = new CFft::complex_t[2 * m_iBlockLength];
    m_ppfIRFeqDomainReal = new float* [m_iIRBlockNum];
    m_ppfIRFreqDomainImag = new float*[m_iIRBlockNum];
    m_ppfProcessedBlockBuffer = new float* [m_iIRBlockNum];
    m_pfInputBlockBuffer = new float[2*m_iBlockLength];
    for (int i = 0; i < m_iIRBlockNum; i++) {
        m_ppfIRFeqDomainReal[i] = new float[m_iBlockLength + 1];
        CVectorFloat::setZero(m_ppfIRFeqDomainReal[i], static_cast<long long int>(m_iBlockLength + 1));
        m_ppfIRFreqDomainImag[i] = new float[m_iBlockLength + 1];
        CVectorFloat::setZero(m_ppfIRFreqDomainImag[i], static_cast<long long int>(m_iBlockLength + 1));
        m_ppfProcessedBlockBuffer[i] = new float[m_iBlockLength];
        CVectorFloat::setZero(m_ppfProcessedBlockBuffer[i], static_cast<long long int>(m_iBlockLength));
        for (int j = 0; j < m_iBlockLength; j++) {
            if (i * iBlockLength + j < m_iIrLength)
                pfIRTemp[j] = pfImpulseResponse[i * iBlockLength + j];
            else
                pfIRTemp[j] = 0;
        }

        for (int j = m_iBlockLength; j < 2 * m_iBlockLength; j++)
            pfIRTemp[j] = 0;
        m_pcFFT->doFft(pfComplexTemp, pfIRTemp);
        m_pcFFT->splitRealImag(m_ppfIRFeqDomainReal[i], m_ppfIRFreqDomainImag[i], pfComplexTemp);
    }
    m_bIsInitialized=true;
    return Error_t::kNoError;
}

Error_t CConvFFT::reset()
{
    if(!m_bIsInitialized)
        return Error_t::kNotInitializedError;
    for (int i =0; i<m_iBlockLength;i++)
    {
        delete[] m_ppfIRFeqDomainReal[i];
        delete[] m_ppfIRFreqDomainImag[i];
        delete[] m_ppfProcessedBlockBuffer[i];
    }
    delete[] m_ppfIRFreqDomainImag;
    delete[] m_ppfIRFeqDomainReal;
    delete[] m_pfInputBlockBuffer;
    delete[] m_ppfProcessedBlockBuffer;

    m_ppfIRFeqDomainReal=0;
    m_ppfIRFreqDomainImag=0;
    m_ppfProcessedBlockBuffer=0;
    m_pfInputBlockBuffer=0;

    delete[] pfIRTemp;
    delete[] pfFFTRealTemp;
    delete[] pfFFTImagTemp;
    delete[] pfCurrentBlockFFTImag;
    delete[] pfCurrentBlockFFTReal;
    delete[] pfIFFTTemp;
    delete[] m_pfImpulseResponse;
    m_pfImpulseResponse=0;
    pfIFFTTemp=0;
    pfIRTemp=0;
    pfFFTImagTemp=0;
    pfFFTRealTemp=0;
    pfCurrentBlockFFTReal=0;
    pfCurrentBlockFFTImag=0;

    delete[] pfComplexTemp;
    pfComplexTemp = 0;

    CFft::destroyInstance(m_pcFFT);
    m_pcFFT=0;

    return Error_t::kNoError;

}
//    ///PseudoCode
//    // outer loop iterates through the ilength of Buffers
//    //Set a readBlockIDx=0 and set WriteBlockIdx = 0, for shift counters in the algorithm
//    //We setup a 2M size input buffer and fill it from 0:M with zeros for block1 and M-2M with x1
//    //We have a processed output matrix of size ypro[No of IR blocks][M], initialized with zeros
//    //The output y[write] = ypro[ReadBlockIdx][write]  (Note: For first block M we will get zeros (Latency of the system))
//    //Once the inputBuffer is filled to 2M we perform the convolution multiplication operation
//    //Innerloop
//    //We receive and processed output of ----
//
//    // 0 + x1*h1  <---- ReadIdx, WriteIdx
//    // 0 + x1*h2
//    // 0 + x1*h3
//    // 0 + x1*h4
//    //Shift second half of the input to the left, to make room for new input Xprocess = [x1:x1]
//
//    //Increment Write Idx, but before you do make sure the readIdx is equal to the write Idx
//    //ReadIdx = 0, WriteIdx = 1
//    //Repeat outer loop process
//    //Have to setup the yprocessed[ReadIdx] = 0, to account for the last process being added to empty mem in the algorithm
//    //After process the processed output is ----
//
//    //0 +x2h4 <---ReadIdx
//    //x1h2+x2h1 <---- WriteIdx
//    //x1h3+x2h2
//    //x1h4+x2h3
//    //Shift the processed input again [x2:x2]
//    //Increment the Write Idx
//    //ReadIdx = 1, WriteIdx = 2
//    // Now
//    //0 +x2h4
//    //x1h2+x2h1 <---ReadIdx
//    //x1h3+x2h2 <---- WriteIdx
//    //x1h4+x2h3
//    //Overlap and Save Implemented
//
//    //https://publications.rwth-aachen.de/record/466561/files/466561.pdf page: 105
Error_t CConvFFT::process(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthBuffers)
{
    return Error_t::kNoError;
}
Error_t CConvFFT::flushBuffer(float *pfOutputBuffer)
{
    float* pfFlushInputBuffer = new float[m_iBlockLength + m_iIrLength - 1];
    CVectorFloat::setZero(pfFlushInputBuffer,static_cast<long long int>(m_iBlockLength + m_iIrLength - 1));
    process(pfOutputBuffer, pfFlushInputBuffer, m_iBlockLength + m_iIrLength - 1);
    delete[] pfFlushInputBuffer;
    return Error_t::kNoError;
}
