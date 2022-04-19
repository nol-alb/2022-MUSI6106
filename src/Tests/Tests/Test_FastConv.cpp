#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Synthesis.h"

#include "Vector.h"
#include "FastConv.h"

#include "gtest/gtest.h"

//TODO: [15] Implement three tests (in Test_FastConv.cpp)
// identity: generate a random IR of length 51 samples and an impulse as input signal at sample index 3; make the input signal
// 10 samples long. Check the correct values of the 10 samples of the output signal.
// flushbuffer: use the same signals as in identity, but now get the full tail of the impulse response and check for correctness
// blocksize: for an input signal of length 10000 samples, implement a test similar to identity with a succession of different
// input/output block sizes (1, 13, 1023, 2048,1,17, 5000, 1897)

//TODO: Duplicate your above tests so they now verify computation in the frequency domain and compensate appropriately for
// latency at different blocksizes.




namespace fastconv_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
    }

    class FastConv: public testing::Test
    {
    protected:
        void SetUp() override
        {
           m_pCFastConv = new CFastConv;
        }

        virtual void TearDown()
        {
            delete m_pCFastConv;
            m_pCFastConv = 0;
        }

        CFastConv *m_pCFastConv = 0;
    };

    TEST_F(FastConv, TimeDomainIdentifyTest)
    {
        float TestImpulse[12] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        TestInput[0] = 1;
        for (int i = 0; i < 12; i++)
        {
            TestImpulse[i] = i;
        }

        m_pCFastConv->init(TestImpulse, 12, 0, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);

        CHECK_ARRAY_CLOSE(TestOutput, TestImpulse, 7, 1e-3);
    }
    TEST_F(FastConv, TimeDomainFlushIdentifyTest)
    {
        float TestImpulse[51] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        float TestFlush[50] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
            // todo: add random values -MIR
            TestImpulse[i] = i;
        }

        m_pCFastConv->init(TestImpulse, 51, 1024, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);
        m_pCFastConv->flushBuffer(TestFlush);

        CHECK_ARRAY_CLOSE(TestOutput + 3, TestImpulse, 7, 1e-3);
        CHECK_ARRAY_CLOSE(TestFlush, TestImpulse + 7, 51 - 7, 1e-3);
    }
}

#endif //WITH_TESTS

