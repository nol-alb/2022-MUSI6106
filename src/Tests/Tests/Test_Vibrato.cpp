#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Vector.h"
#include "Vibrato.h"

#include "gtest/gtest.h"


namespace vibrato_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
    }

    class Lfo : public testing::Test
    {
    protected:
        void SetUp() override
        {
            pLfo = new CLfo();
        }

        virtual void TearDown()
        {
            delete pLfo;
            pLfo = 0;
        }

        CLfo* pLfo = 0;
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

}

#endif //WITH_TESTS
