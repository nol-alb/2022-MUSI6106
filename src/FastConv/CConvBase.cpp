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
    float* pfFlushInputBuffer = new float[m_iIrLength- 1]();
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
    //Initialize fft

    m_iBlockLength=iBlockLength;
    m_iFftLength = iBlockLength << 1;
    m_iIrLength = iLengthOfIr;
    m_iIRBlockNum = static_cast<int>(std::ceil(static_cast<float>(m_iIrLength) / static_cast<float>(m_iBlockLength)));
    CFft::createInstance(m_pcFFT);
    m_pcFFT->initInstance(m_iBlockLength, 2, CFft::kWindowHann,CFft::kNoWindow);
    m_iFftLength=m_pcFFT->getLength(CFft::kLengthFft);
    // Inputbuffer processor memory
    pfFFTRealTemp = new float[m_iBlockLength + 1];
    pfFFTImagTemp = new float[m_iBlockLength + 1];
    pfCurrentBlockFFTReal = new float[m_iBlockLength + 1];
    pfCurrentBlockFFTImag = new float[m_iBlockLength + 1];
    m_ppfProcessedBlockBuffer = new float* [m_iIRBlockNum];
    m_pfInputBlockBuffer = new float[m_iFftLength];

    m_iTailLength = m_iBlockLength * m_iIRBlockNum + m_iBlockLength;
    m_pfTail = new float[m_iTailLength];
    CVectorFloat::setZero(m_pfTail, m_iTailLength);

    // Memory required for Impulse Response
    m_pfImpulseResponse=pfImpulseResponse;
    pfComplexTemp = new CFft::complex_t[m_iFftLength]();
    pfIRTemp = new float[2*m_iBlockLength];
    pfIFFTTemp= new float [m_iFftLength];
    long long int ldbBlockLength = 2*m_iBlockLength;
    m_ppfIRFeqDomainReal = new float*  [m_iIRBlockNum];
    m_ppfIRFreqDomainImag = new float* [m_iIRBlockNum];
    for(int i=0;i<m_iIRBlockNum;i++) {
        //Set ProcessedBlock zeros
        m_ppfProcessedBlockBuffer[i] = new float[m_iBlockLength];
        CVectorFloat::setZero(m_ppfProcessedBlockBuffer[i], static_cast<long long int>(m_iBlockLength));
        //Impulse Processing
        CVectorFloat::setZero(pfIRTemp, ldbBlockLength);
        //Fill IRtemp with zeros and then move blocklength of Impulseresponse into IRTemp, IR temp is now 2blockLength
        for (int j = 0; j < m_iBlockLength; j++) {
            if (i * iBlockLength + j < m_iIrLength)
                pfIRTemp[j] = pfImpulseResponse[i * iBlockLength + j];
            else
                pfIRTemp[j] = 0;
        }
        //Creat real and Imaginary memory for IR
        m_ppfIRFeqDomainReal[i] = new float[m_iBlockLength + 1];
        CVectorFloat::setZero(m_ppfIRFeqDomainReal[i], static_cast<long long int>(m_iBlockLength + 1));
        m_ppfIRFreqDomainImag[i] = new float[m_iBlockLength + 1];
        CVectorFloat::setZero(m_ppfIRFreqDomainImag[i], static_cast<long long int>(m_iBlockLength + 1));
        m_pcFFT->doFft(pfComplexTemp, pfIRTemp);
        CVectorFloat::mulC_I(pfComplexTemp, m_iFftLength, m_iFftLength);
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
    delete[] m_pfTail;
    m_pfTail=0;
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

Error_t CConvFFT::process(float *pfOutputBuffer, const float *pfInputBuffer, int iLengthBuffers)
{
    if (!m_bIsInitialized)
        return Error_t::kFunctionIllegalCallError;

    if (!pfInputBuffer)
        return Error_t::kFunctionInvalidArgsError;

    if (!pfOutputBuffer)
        return Error_t::kFunctionInvalidArgsError;

    if (iLengthBuffers <= 0)
        return Error_t::kFunctionInvalidArgsError;
    int WriteIdx = 0;
    //Set a writeIdx to zero, to fill the inputprocessing buffer until the minimum length (blockLength)
    for (int i=0; i<iLengthBuffers;i++)
    {
        //Check if the iLengthBuffers or iBlockLength is smaller
        int processLength = std::min<int>(iLengthBuffers, m_iBlockLength);
        //Set zeros to m_pfInputProcess buffer
        CVectorFloat::setZero(m_pfInputBlockBuffer,m_iFftLength);
        //fill the inputblockbuffer of size 2BlockSize until the processing size is met
        m_pfInputBlockBuffer[WriteIdx]= pfInputBuffer[i];
        //Fill the output buffer with values from the current readBlock
        pfOutputBuffer[i] = m_ppfProcessedBlockBuffer[m_iReadBlockNo][WriteIdx];
        WriteIdx++;
        if(WriteIdx==processLength)
        {
            WriteIdx=0;
            //Set the processBlock that is read by the output to zero to make room for new convolved block
            CVectorFloat::setZero(m_ppfProcessedBlockBuffer[m_iReadBlockNo],static_cast<long long int>(m_iBlockLength));
            //Set the tempComplex array to zero
            CVectorFloat::setZero(pfComplexTemp, m_iFftLength);
            //Perform FFT on the blocked Input Buffer
            m_pcFFT->doFft(pfComplexTemp, m_pfInputBlockBuffer);
            //Split the Real and Imaginary parts
            m_pcFFT->splitRealImag(pfCurrentBlockFFTReal, pfCurrentBlockFFTImag, pfComplexTemp);
            //Iterate through all the IR blocks
            for (int iIrBlock = 0; iIrBlock < m_iIRBlockNum; iIrBlock++)
            {
                //perform complex multiplication
                complexMultiplication(pfFFTRealTemp, pfFFTImagTemp,
                                      pfCurrentBlockFFTReal, pfCurrentBlockFFTImag,
                                      m_ppfIRFeqDomainReal[iIrBlock], m_ppfIRFreqDomainImag[iIrBlock]);
                //Reusing complexTemp
                CVectorFloat::setZero(pfComplexTemp, m_iFftLength);
                //Merge Real and Imaginary parts
                m_pcFFT->mergeRealImag(pfComplexTemp, pfFFTRealTemp, pfFFTImagTemp);

                m_pcFFT->doInvFft(pfIFFTTemp, pfComplexTemp);
                //Normalize ifft output
                CVectorFloat::mulC_I(pfIFFTTemp, 1.0f / m_iFftLength, m_iFftLength);
                //Overlap and add into the ptocessedBlock buffer
                //for h1,h2,h3,h4
                // --> x1*h1 -->WriteIdx, ReadIdx   --> 0+x2*h4                                 --> x2*h4+x3*h3                                     -->x2*h4+x3*h3+x4*h2
                //     x1*h2                            x1*h2+x2*h1 --> WriteBlockIdx, ReadIdx      0+x3*h4                                            x3*h4+x4*h3
                //     x1*h3                            x1*h3+x2*h2                                 x1*h3+x2*h2+x3*h1 -->WriteBlocIdx, ReadBlockIdx    0+x4*h4
                //     x1*h4                            x1*h4+x2*h3                                 x1*h4+x2*h3+x3*h2                                  x1*h4+x2*h3+x3*h2+x4*h1
                int process_iWriteBlockNum = (m_iWriteBlockNo + iIrBlock) % m_iIRBlockNum;
                for (int k = 0; k < m_iBlockLength; k++)
                {
                    m_ppfProcessedBlockBuffer[process_iWriteBlockNum][k] += pfIFFTTemp[k];
                }

            }
            m_iReadBlockNo = m_iWriteBlockNo;
            m_iWriteBlockNo = (m_iWriteBlockNo + 1) % m_iIRBlockNum;


        }


        }

    return Error_t::kNoError;
}
Error_t CConvFFT::flushBuffer(float *pfOutputBuffer)
{
    float* pfFlushInputBuffer = new float[m_iBlockLength + m_iIrLength - 1]();
    CVectorFloat::setZero(pfFlushInputBuffer,static_cast<long long int>(m_iBlockLength + m_iIrLength - 1));
    process(pfOutputBuffer, pfFlushInputBuffer, m_iBlockLength + m_iIrLength - 1);
    delete[] pfFlushInputBuffer;
    return Error_t::kNoError;
}
