#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#include "RingBuffer.h"

class CLfo
{

	enum LfoParam_t 
	{
		kAmplitude,
		kFrequency,

		kNumParams
	};

public:

	CLfo() {};
	~CLfo() {};

	void setParam(LfoParam_t param_t, float fValue)
	{

	}

	float getParam(LfoParam_t param_t) const
	{

	}

	float process()
	{
		float fCurrentValue = m_pWavetable.get(m_fCurrentIndex);
		m_fCurrentIndex += m_fTableDelta;
		return m_fAmplitude * fCurrentValue;
	}

private:

	const int m_iWavetableSize = 1 << 7;
	const CRingBuffer<float> m_pWavetable = CRingBuffer<float>(m_iWavetableSize);
	float m_fTableDelta = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fAmplitude = 0.0f;

	void setFrequency(float fValue)
	{

	}

	void setGain(float fValue)
	{

	}

};

#endif