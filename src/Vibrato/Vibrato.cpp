
#include "Vibrato.h"
#include "RingBuffer.h"
#include "Lfo.h"
#include "MUSI6106Config.h"
#include <iostream>

using std::cout;
using std::endl;

CVibrato::CVibrato() :
    m_bIsInitialised(false),
    m_fFrequency(0),
    m_iNumChannels(0),
    m_ptBuff(0),
    m_pLFO(0),
    m_fDelayinSamples(0),
    m_fModFrequencyinHz(0),
    m_fSampleRateinHz(0),
    m_fWidthInSamples(0)
{
    // this should never hurt
    this->reset ();
}
CVibrato::~CVibrato() {
    this->reset();
}

Error_t CVibrato::create(CVibrato*& pCVibrato)
{
    pCVibrato = new CVibrato() ;
    return Error_t::kNoError;
}

Error_t CVibrato::destroy(CVibrato*& pCVibrato)
{
    if (!pCVibrato)
        return Error_t::kUnknownError;
    pCVibrato->reset();
    delete pCVibrato;
    pCVibrato=0;

    return Error_t::kNoError;


}

Error_t CVibrato::reset()
{
    delete m_pLFO;
    m_pLFO=0;
    return Error_t::kNoError;
}

Error_t CVibrato::setParam(ParamVibrato vParam, float fParamValue)
{
    return Error_t::kNoError;
}


