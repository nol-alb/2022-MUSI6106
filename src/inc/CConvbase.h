//
// Created by Noel  Alben on 4/28/22.
//

#ifndef MUSI6106_CCONVBASE_H
#define MUSI6106_CCONVBASE_H

#endif //MUSI6106_CCONVBASE_H

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "FastConv.h"
#include <string>
#include "Fft.h"
#include "Vector.h"

class CConvbase
{
    //Use virtual functions when you have a base class function that will be overridden by the child class
public:
    CConvbase(void);
    virtual ~CConvbase(void);
    virtual Error_t init(float*  pfImpulseResponse, int iLengthOfIr, int iBlockLength=8192) = 0;
    virtual Error_t reset() = 0;
    // Equating to zero makes it a pure virtual function
    virtual Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthBuffers)=0;
    virtual Error_t flushBuffer(float *pfOutputBuffer) = 0;
protected:
    int m_iIrLength =0,
    m_iBlockLength = 0;

    bool m_bIsInitialized = false;

};

class CConvTime : public CConvbase
{
public:
    CConvTime(void) {};
    virtual ~CConvTime(void) {};
    Error_t init(float* pfImpulseResponse, int iLengthOfIr, int iBlockLength) override;
    Error_t reset() override;

    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthBuffers) override;
    Error_t flushBuffer(float* pfOutputBuffer) override;

private:
    float* m_pfImpulseResponse =0;
    CRingBuffer<float>* m_pCRingBuff = 0;


};
class CConvFFT : public CConvbase
{
public:
    CConvFFT(void) {};
    virtual ~CConvFFT(void) {};

    Error_t init(float* pfImpulseResponse, int iLengthOfIr, int iBlockLength) override;
    Error_t reset() override;

    Error_t process(float* pfOutputBuffer, const float* pfInputBuffer, int iLengthBuffers) override;
    Error_t flushBuffer(float* pfOutputBuffer) override;

private:
    void complexMultiplication(float* pfOutReal, float* pfOutImag, const float* pfSignalReal, const float* pfSignalImag, const float* pfIRReal, const float* pfIRImag)
    {
        for (int i = 0; i < m_iBlockLength+1 ; i++)
        {
            pfOutReal[i] = (pfSignalReal[i] * pfIRReal[i] - pfSignalImag[i] * pfIRImag[i]) * 2 * m_iBlockLength;
            pfOutImag[i] = (pfSignalReal[i] * pfIRImag[i] + pfSignalImag[i] * pfIRReal[i]) * 2 * m_iBlockLength;
        }
    }

    int m_iIRBlockNum=0;
    int m_iBlockLength=0;
    int m_iIrLength=0;
    int m_iFftLength=0;
    float* m_pfImpulseResponse=0;

    int m_iReadBlockNo=0;
    int m_iWriteBlockNo=0;
    int m_WriteNo=0;

    float** m_ppfIRFeqDomainReal =0;
    float** m_ppfIRFreqDomainImag=0;
    float* m_pfInputBlockBuffer=0;
    float** m_ppfProcessedBlockBuffer=0;

    float* pfFFTRealTemp=0;
    float* pfFFTImagTemp=0;

    float* pfCurrentBlockFFTReal = 0;
    float* pfCurrentBlockFFTImag = 0;

    float* pfIRTemp = 0;
    float* pfIFFTTemp=0;



    CFft* m_pcFFT = 0;
    CFft::complex_t* pfComplexTemp = 0;


};