#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__
#include "RingBuffer.h"
#include "Lfo.h"
#include "ErrorDef.h"

/*
 This is the Vibrato Interface:
 A user has control over
    - What the initial delayInSeconds they need the effect for
    - What width of widthInSecs the Vibrato is processed
    - The frequency of the LFO
    - Type of LFO?
User methods:
 1. Create                                |
 2. Destroy                               |  Can be handled by a constructor/Destructor I guess
 3. Initialise all the Vibrato Parameters |
 4. Reset The buffer and Vibrato
 5. Set Get and Processes
 The interface is designed similar to the CCombFilterIf



 */

class CVibrato
{
public:
    enum ParamVibrato{
        kDelayInSec,
        kWidthInSec,
        kLFOFreqInHz,
        kSampleRate,


        kNumVibratoParams
    };
    static Error_t create (CVibrato*& pCVibrato);
    static Error_t destroy (CVibrato*& pCVibrato);

    //Initialise the relevant parameters as shown in Matlab and Vibrato Requirements

    Error_t init (float fdelayInSec, float fmodWidthInSec, float flfoFreqInHz, float fSampleFreqInHz, int iNumChannels);
    // Reset the Vibrato to Default Values

    //Set the parameters of the Vibrato Enum
    Error_t setParam(ParamVibrato vParam, float fParamValue);

    //Get the parameter values User Specifies
    float getParam(ParamVibrato vParam) const;

    //Apply the Vibrato Effect to inputBuffer and write to the outputBuffer renamed iNumFrames to pBlockSize

    Error_t process(float **ppfInputBuffer, float **ppfOutputbuffer, int pBlockSize);







protected:
    CVibrato();
    virtual ~CVibrato();
    Error_t reset();

private:
    bool m_bIsInitialised;

    CLfo *m_pLFO;                   //Handle the LFO
    CRingBuffer<float> **m_ptBuff; //Handle Ring Buffer

    float m_fFrequency;
    float m_fSampleRateinSamples;
    float m_fModFrequencyinSamples;
    float m_fWidthInSamples;
    float m_fDelayinSamples;

    int m_iNumChannels;

};

#endif // __Vibrato_hdr__
