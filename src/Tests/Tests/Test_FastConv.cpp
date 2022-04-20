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

    class FastConv : public testing::Test
    {
    protected:
        void SetUp() override
        {
            m_pCFastConv = new CFastConv;
        }

        virtual void TearDown() override
        {
            delete m_pCFastConv;
            m_pCFastConv = 0;
        }

        CFastConv* m_pCFastConv = 0;
    };

    TEST_F(FastConv, TimeDomainIdentityTest)
    {
        float TestImpulse[51] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        float CheckOutput[60] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
            TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
            TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
            CheckOutput[i + 3] = TestImpulse[i];
        }


        m_pCFastConv->init(TestImpulse, 51, 0, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);


        CHECK_ARRAY_CLOSE(CheckOutput, TestOutput, 10, 1e-3);
    }
    TEST_F(FastConv, TimeDomainFlushBufferTest)
    {
        float TestImpulse[51] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        float TestFlush[50] = { 0 };
        float CheckOutput[60] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
            TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
            TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
            CheckOutput[i + 3] = TestImpulse[i];
        }


        m_pCFastConv->init(TestImpulse, 51, 0, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);
        m_pCFastConv->flushBuffer(TestFlush);

        CHECK_ARRAY_CLOSE(TestOutput, CheckOutput, 10, 1e-3);
        CHECK_ARRAY_CLOSE(TestFlush, TestImpulse + 7, 51 - 7, 1e-3);

    }

    TEST_F(FastConv, TimeDomainBlockSizeTest)
    {
        float TestImpulse[51] = { 0 };
        float TestInput[10000] = { 0 };
        float TestOutput[10000] = { 0 };
        int BufferSize[8] = { 1, 13, 1023, 2048, 1, 17, 5000, 1897 };
        int InputStartIdx[8] = { 0 }; // All of the buffersizes add up to 10000, so we can just start the reading of the input at shifted positions

        // generate IR of length 51 samples
        for (int i = 0; i < 51; i++)
        {
            TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
            TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
        }

        m_pCFastConv->init(TestImpulse, 51, 1024, CFastConv::kTimeDomain);
        for (int i = 0; i < 8; i++)
        {
            if (i == 0) {
                InputStartIdx[i] = 0;
            }
            else {
                InputStartIdx[i] = InputStartIdx[i-1] + BufferSize[i-1];
            }

            // set all beginnings of new block sizes to 1 for easy testing of convolution
            TestInput[InputStartIdx[i]] = 1;
            // reinitialize the convolution every time bc this is the easiest way to reset the pointers in the IR
            m_pCFastConv->init(TestImpulse, 51, 1024, CFastConv::kTimeDomain);

            m_pCFastConv->process(TestOutput + InputStartIdx[i], TestInput + InputStartIdx[i], BufferSize[i]);
            CHECK_ARRAY_CLOSE(TestOutput + InputStartIdx[i], TestImpulse, std::min(BufferSize[i], 51), 1e-3);

        }
    }

    //TEST_F(FastConv, TimeDomainBlockingTest)
    //{
    //    float* TestImpulse = new float[51];
    //    for (int i=0; i<51;i++)
    //    {
    //        TestImpulse[i]=0;
    //    }
    //    float* TestInput =  new float[10000];
    //    float* TestOutput = new float[10000];
    //    for (int i=0; i<10000;i++)
    //    {
    //        TestInput[i]=0;
    //        TestOutput[i]=0;
    //    }
    //    int BufferSize[8] = {1, 13, 1023, 2048, 1, 17, 5000, 1897 };
    //    int InputStartIdx[8] = { 0 }; // All of the buffersizes add up to 10000, so we can just start the reading of the input at shifted positions
    //    for (int i=1; i<8;i++)
    //    {
    //        InputStartIdx[i] = InputStartIdx[i-1]+BufferSize[i-1];
    //    }
    //   
    //    for (int i = 0; i < 51; i++)
    //    {
    //        TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
    //        TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
    //    }

    //    for (int i = 0; i < 8; i++)
    //    {
    //        m_pCFastConv->init(TestImpulse, 51, 0, CFastConv::kTimeDomain);

    //        TestInput[InputStartIdx[i]] = 1;
    //        m_pCFastConv->process(TestOutput + InputStartIdx[i], TestInput + InputStartIdx[i], BufferSize[i]);
    //        CHECK_ARRAY_CLOSE(TestOutput+InputStartIdx[i], TestImpulse, BufferSize[i]-1, 1e-3);

    //    }
    //    delete [] TestInput;
    //    delete [] TestImpulse;
    //    delete [] TestOutput;
    //    TestImpulse = nullptr;
    //    TestInput = nullptr;
    //    TestOutput= nullptr;


    //}
}

#endif //WITH_TESTS

