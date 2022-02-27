#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Vector.h"
#include "Vibrato.h"
#include "Synthesis.h"

#include "gtest/gtest.h"


namespace vibrato_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
    }
    class CVibratoTests : public testing::Test
    {
    public:
        CVibrato *p_CVibratoTest=0;
        float** m_ppfInBuffer = 0;
        float** m_ppfOutBuffer = 0;
        int m_iBufferChannels = 2;
        int m_iBufferLength = 1000;
        int m_iSampleFreq = 44100;

        // You can remove any or all of the following functions if their bodies would
        // be empty.

        CVibratoTests() 
        {

        }

        ~CVibratoTests() override 
        {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).

            CVibrato::create(p_CVibratoTest);
            m_ppfInBuffer = new float*[m_iBufferChannels];
            m_ppfOutBuffer = new float*[m_iBufferChannels];
            for (int channel = 0; channel < m_iBufferChannels; channel++)
            {
                m_ppfInBuffer[channel] = new float[m_iBufferLength] {};
                m_ppfOutBuffer[channel] = new float[m_iBufferLength] {};
            }

        }

        virtual void TearDown() override{
            CVibrato::destroy(p_CVibratoTest);
            for (int channel = 0; channel < m_iBufferChannels; channel++)
            {
                delete[] m_ppfInBuffer[channel];
                delete[] m_ppfOutBuffer[channel];
            }
            delete[] m_ppfInBuffer;
            delete[] m_ppfOutBuffer;
            m_ppfInBuffer = 0;
            m_ppfOutBuffer = 0;
        }

        // Class members declared here can be used by all tests in the test suite
        // for Foo.
    };
    TEST_F(CVibratoTests, CorrectInputOutputHandles)
    {
        int iNumChannels = 1;
        float fTestSampleRateInHz= 44100;
        float fTestDelay=0.1f;
        float fTestModInHz= 15;
        float fTestWidth=0.01f;
        EXPECT_EQ(p_CVibratoTest->init(fTestDelay,fTestWidth,fTestModInHz,fTestSampleRateInHz,iNumChannels),Error_t::kNoError);
        EXPECT_EQ(p_CVibratoTest->getParam(CVibrato::ParamVibrato::kLFOFreqInHz),15);
        EXPECT_EQ(p_CVibratoTest->getParam(CVibrato::ParamVibrato::kSampleRate),44100);
        EXPECT_EQ(p_CVibratoTest->getParam(CVibrato::ParamVibrato::kWidthInSec),0.01f);
        iNumChannels = 2;
        fTestSampleRateInHz= 44100;
        fTestDelay=1.5f;
        fTestModInHz= 15;
        fTestWidth=0.015f;
        EXPECT_EQ(p_CVibratoTest->init(fTestDelay,fTestWidth,fTestModInHz,fTestSampleRateInHz,iNumChannels),Error_t::kNoError);
        EXPECT_EQ(p_CVibratoTest->getParam(CVibrato::ParamVibrato::kWidthInSec),0.015f);
    }
    TEST_F(CVibratoTests, OutBoundsInputOutputHandles)
    {
        int iNumChannels = 1;
        float fTestSampleRateInHz= -44100;
        float fTestDelay=-0.1f;
        float fTestModInHz= -15;
        float fTestWidth=0.01f;
        EXPECT_EQ(p_CVibratoTest->init(fTestDelay,fTestWidth,fTestModInHz,fTestSampleRateInHz,iNumChannels),Error_t::kFunctionInvalidArgsError);

    }

    TEST_F(CVibratoTests, DCstaysDC)
    {
        for (int channel = 0; channel < m_iBufferChannels; channel++)
            CSynthesis::generateDc(m_ppfInBuffer[channel], m_iBufferLength, 1);
        float fWidth = 0.0000025;
        float fDelayInSec = 1000;

        p_CVibratoTest->init(fDelayInSec, fWidth, 1, 44100, m_iBufferChannels);
        p_CVibratoTest->process(m_ppfInBuffer, m_ppfOutBuffer, m_iBufferLength);
        int check_start_post = static_cast<int>(2+fDelayInSec*m_iSampleFreq+fWidth*2*m_iSampleFreq);
        for (int channel = 0; channel < m_iBufferChannels; channel++)
            for (int val = check_start_post; val<m_iBufferLength; val++)
            {
                EXPECT_NEAR(m_ppfInBuffer[channel][val], m_ppfOutBuffer[channel][val],0);
            }
    }


    class Lfo : public testing::Test
    {
    protected:
        void SetUp() override
        {
            m_pLfo = new CLfo();
            m_pfLfoBuffer = new float[m_iBufferLength] {};
            m_pfSynthesisBuffer = new float[m_iBufferLength] {};
        }

        virtual void TearDown()
        {
            delete m_pLfo;
            delete[] m_pfLfoBuffer;
            delete[] m_pfSynthesisBuffer;
            m_pLfo = 0;
            m_pfLfoBuffer = 0;
            m_pfSynthesisBuffer = 0;
        }

        void compareSinusoids(float fSampleRate, float fFrequency, float fAmplitude)
        {
            m_pLfo->resetPhase();

            m_pLfo->setParam(CLfo::CLfo::kSampleRate, fSampleRate);
            m_pLfo->setParam(CLfo::CLfo::kFrequency, fFrequency);
            m_pLfo->setParam(CLfo::CLfo::kAmplitude, fAmplitude);

            CSynthesis::generateSine(m_pfSynthesisBuffer, fFrequency, fSampleRate, m_iBufferLength, fAmplitude);

            for (int i = 0; i < m_iBufferLength; i++)
                m_pfLfoBuffer[i] = m_pLfo->process();

            CHECK_ARRAY_CLOSE(m_pfLfoBuffer, m_pfSynthesisBuffer, m_iBufferLength, 1E-3);
        }

        CLfo* m_pLfo = 0;
        float* m_pfLfoBuffer = 0;
        float* m_pfSynthesisBuffer = 0;
        int m_iBufferLength = 1000;
    };

    TEST_F(Lfo, HandlesOutOfBoundsInput)
    {
        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kSampleRate, -1) , Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kSampleRate, -11020), Error_t::kFunctionInvalidArgsError);

        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kFrequency, -1), Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kFrequency, -28381), Error_t::kFunctionInvalidArgsError);

        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kAmplitude, -2), Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kAmplitude, 3), Error_t::kFunctionInvalidArgsError);
    }

    TEST_F(Lfo, SetParametersCorrectly)
    {
        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kSampleRate, 44100), Error_t::kNoError);
        EXPECT_EQ(m_pLfo->getParam(CLfo::LfoParam_t::kSampleRate), 44100);

        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kFrequency, 440), Error_t::kNoError);
        EXPECT_EQ(m_pLfo->getParam(CLfo::LfoParam_t::kFrequency), 440);

        EXPECT_EQ(m_pLfo->setParam(CLfo::LfoParam_t::kAmplitude, 0.5), Error_t::kNoError);
        EXPECT_EQ(m_pLfo->getParam(CLfo::LfoParam_t::kAmplitude), 0.5);
    }

    TEST_F(Lfo, ReturnCorrectSinusoid)
    {
        int freqs[10]{ 1,2,3,4,5,6,7,8,9,10 };
        for (int i = 0; i < 10; i++)
            compareSinusoids(44100, freqs[i], 1.0);

        float amps[7]{ -1.0, -0.5, -0.25, 0, 0.25, 0.5, 1.0 };
        for (int i = 0; i < 7; i++)
            compareSinusoids(44100, 440, amps[i]);

        int sampleRates[4]{ 11025, 22050, 44100, 48000};
        for (int i = 0; i < 4; i++)
            compareSinusoids(sampleRates[i], 440, 1.0);
    }


    class RingBuffer : public testing::Test
    {
    protected:
        void SetUp() override
        {

        }

        virtual void TearDown()
        {

        }


    };

}

#endif //WITH_TESTS
