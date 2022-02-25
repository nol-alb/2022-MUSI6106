#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

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
		float angleDelta = M_PI / (float)(m_pWavetable.getLength() - 1);
		float currentAngle = 0.0;

		for (int i = 0; i < m_pWavetable.getLength(); i++)
		{
			float sample = std::sin(currentAngle);
			m_pWavetable.putPostInc(sample);
			currentAngle += angleDelta;
		}
	}

	~CLfo() {};

	void setParam(LfoParam_t param_t, float fValue)
	{
		switch (param_t)
		{
		case LfoParam_t::kAmplitude:
			setGain(fValue);
		case LfoParam_t::kFrequency:
			setFrequency(fValue);
		case LfoParam_t::kSampleRate:
			setSampleRate(fValue);
		}
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
		float fCurrentValue = m_pWavetable.get(m_fCurrentIndex);
		m_fCurrentIndex += m_fTableDelta;
		return m_fAmplitude * fCurrentValue;
	}

private:

	const int m_iWavetableSize = 1 << 9;
	CRingBuffer<float> m_pWavetable = CRingBuffer<float>(m_iWavetableSize);
	float m_fTableDelta = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fAmplitude = 0.0f;
	float m_fFrequency = 0.0f;
	float m_fSampleRate = 0.0f;

	void setFrequency(float fValue)
	{
		m_fFrequency = fValue;
		m_fTableDelta = (m_fSampleRate == 0) ? m_fFrequency / m_fSampleRate : 0;
	}

	void setGain(float fValue)
	{
		m_fAmplitude = fValue;
	}

	void setSampleRate(float fValue)
	{
		m_fSampleRate = fValue;
		setFrequency(m_fFrequency);
	}

};

#endif