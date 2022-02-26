#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

#include "ErrorDef.h"
#include "RingBuffer.h"

class CLfo
{

	enum LfoParam_t 
	{
		kAmplitude,
		kFrequency,
		kSampleRate,

		kNumParams
	};

public:

	CLfo() 
	{
        m_pWavetable= new CRingBuffer<float>(m_iWavetableSize);

		float angleDelta = M_PI / (float)(m_pWavetable->getLength() - 1);
		float currentAngle = 0.0;

		for (int i = 0; i < m_pWavetable->getLength(); i++)
		{
			float sample = std::sin(currentAngle);
			m_pWavetable->putPostInc(sample);
			currentAngle += angleDelta;
		}
	}

	~CLfo() 
	{
		delete m_pWavetable;
	};

	Error_t setParam(LfoParam_t param_t, float fValue)
	{
		switch (param_t)
		{
		case LfoParam_t::kAmplitude:
			return setGain(fValue);
		case LfoParam_t::kFrequency:
			return setFrequency(fValue);
		case LfoParam_t::kSampleRate:
			return setSampleRate(fValue);
		}
		return Error_t::kFunctionInvalidArgsError;
	}

	float getParam(LfoParam_t param_t) const
	{
		switch (param_t)
		{
		case LfoParam_t::kAmplitude:
			return m_fAmplitude;
		case LfoParam_t::kFrequency:
			return m_fFrequency;
		case LfoParam_t::kSampleRate:
			return m_fSampleRate;
		}
	}

	float process()
	{
		float fCurrentValue = m_pWavetable->get(m_fCurrentIndex);
		m_fCurrentIndex += m_fTableDelta;
		return m_fAmplitude * fCurrentValue;
	}

private:

	const int m_iWavetableSize = 1 << 9;
	CRingBuffer<float> *m_pWavetable = 0;
	float m_fTableDelta = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fAmplitude = 0.0f;
	float m_fFrequency = 0.0f;
	float m_fSampleRate = 0.0f;

	Error_t setFrequency(float fValue)
	{
		if (fValue < 0)
			return Error_t::kFunctionInvalidArgsError;
		m_fFrequency = fValue;
		m_fTableDelta = (m_fSampleRate == 0) ? m_fFrequency / m_fSampleRate : 0;
		return Error_t::kNoError;
	}

	Error_t setGain(float fValue)
	{
		if (fValue < -1.0 || fValue > 1.0)
			return Error_t::kFunctionInvalidArgsError;
		m_fAmplitude = fValue;
		return Error_t::kNoError;
	}

	Error_t setSampleRate(float fValue)
	{
		if (fValue < 0)
			return Error_t::kFunctionInvalidArgsError;
		m_fSampleRate = fValue;
		setFrequency(m_fFrequency);
		return Error_t::kNoError;
	}

};

#endif