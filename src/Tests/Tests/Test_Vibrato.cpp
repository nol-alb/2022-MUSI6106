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
    protected:
        CVibrato *p_CVibratoTest=0;
        float** InputTestBuff;
        float** OutputTestBuff;
        int iNumChannels;
        float fTestSampleRateInHz;
        float fTestWidth;
        float fTestDelay;
        float fTestModInHz;
        int iTestSigLength;

        // You can remove any or all of the following functions if their bodies would
        // be empty.

        CVibratoTests() {
            // You can do set-up work for each test here.
            iNumChannels = 1;
            fTestSampleRateInHz= 44100;
            fTestDelay=0.1f;
            fTestModInHz= 15;
            fTestWidth=0.01f;
            CVibrato::create(p_CVibratoTest);
            p_CVibratoTest->init(fTestDelay,fTestWidth,fTestModInHz,fTestSampleRateInHz,iNumChannels);
            p_CVibratoTest->setParam(CVibrato::kWidthInSec, fTestWidth);
        }

        ~CVibratoTests() override {
            //// You can do clean-up work that doesn't throw exceptions here.
            //CVibrato::destroy(p_CVibratoTest);
            //for (int i = 0; i <iNumChannels; i++)
            //{
            //    delete [] InputTestBuff[i];
            //    delete [] OutputTestBuff[i];
            //}
            //delete [] InputTestBuff;
            //delete [] OutputTestBuff;
            //InputTestBuff = 0;
            //OutputTestBuff = 0;
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        void TearDown() override {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        // Class members declared here can be used by all tests in the test suite
        // for Foo.
    };

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

    class Lfo : public testing::Test
    {
    protected:
        void SetUp() override
        {
            pLfo = new CLfo();
            pfLfoBuffer = new float[iBufferLength];
            pfSynthesisBuffer = new float[iBufferLength];
        }

        virtual void TearDown()
        {
            delete pLfo;
            delete pfLfoBuffer;
            delete pfSynthesisBuffer;
            pLfo = 0;
            pfLfoBuffer = 0;
            pfSynthesisBuffer = 0;
        }

        void compareSinusoids(float fSampleRate, float fFrequency, float fAmplitude)
        {
            pLfo->resetPhase();

            pLfo->setParam(CLfo::CLfo::kSampleRate, fSampleRate);
            pLfo->setParam(CLfo::CLfo::kFrequency, fFrequency);
            pLfo->setParam(CLfo::CLfo::kAmplitude, fAmplitude);

            CSynthesis::generateSine(pfSynthesisBuffer, fFrequency, fSampleRate, iBufferLength, fAmplitude);

            for (int i = 0; i < iBufferLength; i++)
                pfLfoBuffer[i] = pLfo->process();

            CHECK_ARRAY_CLOSE(pfLfoBuffer, pfSynthesisBuffer, iBufferLength, 1E-3);
        }

        CLfo* pLfo = 0;
        float* pfLfoBuffer = 0;
        float* pfSynthesisBuffer = 0;
        int iBufferLength = 1000;
    };

    TEST_F(Lfo, HandlesOutOfBoundsInput)
    {
        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kSampleRate, -1) , Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kSampleRate, -11020), Error_t::kFunctionInvalidArgsError);

        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kFrequency, -1), Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kFrequency, -28381), Error_t::kFunctionInvalidArgsError);

        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kAmplitude, -2), Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kAmplitude, 3), Error_t::kFunctionInvalidArgsError);
    }

    TEST_F(Lfo, SetParametersCorrectly)
    {
        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kSampleRate, 44100), Error_t::kNoError);
        EXPECT_EQ(pLfo->getParam(CLfo::LfoParam_t::kSampleRate), 44100);

        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kFrequency, 440), Error_t::kNoError);
        EXPECT_EQ(pLfo->getParam(CLfo::LfoParam_t::kFrequency), 440);

        EXPECT_EQ(pLfo->setParam(CLfo::LfoParam_t::kAmplitude, 0.5), Error_t::kNoError);
        EXPECT_EQ(pLfo->getParam(CLfo::LfoParam_t::kAmplitude), 0.5);
    }

    TEST_F(Lfo, ReturnCorrectSinusoid)
    {
        int freqs[10]{ 1,2,3,4,5,6,7,8,9,10 };
        for (int i = 0; i < 10; i++)
            compareSinusoids(44100, freqs[i], 1.0);

        int amps[7]{ -1.0, -0.5, -0.25, 0, 0.25, 0.5, 1.0 };
        for (int i = 0; i < 7; i++)
            compareSinusoids(44100, 440, amps[i]);

        int sampleRates[4]{ 11025, 22050, 44100, 48000};
        for (int i = 0; i < 4; i++)
            compareSinusoids(sampleRates[i], 440, 1.0);
    }

    //TEST_F(CVibratoTests, DCstaysDC)
    //{
    //    int iNumFrames = 1000;
    //    int iNumChannels = 1;
    //    float** ppfInBuffer = new float* [iNumChannels];
    //    float** ppfOutBuffer = new float* [iNumChannels];
    //    for (int channel = 0; channel < iNumChannels; channel++)
    //    {
    //        ppfInBuffer[channel] = new float[iNumFrames];
    //        ppfOutBuffer[channel] = new float[iNumFrames];
    //    }

    //    for (int channel = 0; channel < iNumChannels; channel++)
    //        CSynthesis::generateDc(ppfInBuffer[channel], iNumFrames, 1);

    //    p_CVibratoTest->process(ppfInBuffer, ppfOutBuffer, iNumFrames);

    //    for (int channel = 0; channel < iNumChannels; channel++)
    //        CHECK_ARRAY_CLOSE(ppfInBuffer[channel], ppfOutBuffer[channel], 1000, 0);

    //    for (int channel = 0; channel < iNumChannels; channel++)
    //    {
    //        delete[] ppfInBuffer[channel];
    //        delete[] ppfOutBuffer[channel];
    //    }
    //    delete[] ppfInBuffer;
    //    delete[] ppfOutBuffer;
    //}
}

#endif //WITH_TESTS
