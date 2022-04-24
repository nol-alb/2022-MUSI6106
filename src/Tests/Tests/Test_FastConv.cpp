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
        virtual void SetUp() 
        {
            m_pCFastConv = new CFastConv;
            TestImpulse[0]=0;
 
            for (int i = 1; i < 51; i++)
            {
                TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
               // TestImpulse[i] = i;//TestImpulse[i] * 2.0 - 1.0;
            }
        }

        virtual void TearDown() 
        {
            delete m_pCFastConv;
            m_pCFastConv = 0;
            for (int i = 0; i < 51; i++) {
                TestImpulse[i] = 0;
            }
        }

        CFastConv* m_pCFastConv = 0;
        float TestImpulse[51] = { 0 };


    };


    ////////////////////////////////////////////////////
    // TIME DOMAIN TESTS
    ////////////////////////////////////////////////////
    TEST_F(FastConv, TimeDomainIdentityTest)
    {
        // float TestImpulse[51] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        float CheckOutput[60] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
        //    TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
          //  TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
            CheckOutput[i + 3] = TestImpulse[i];
        }


        m_pCFastConv->init(TestImpulse, 51, 1024, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);


        CHECK_ARRAY_CLOSE(CheckOutput, TestOutput, 10, 1e-3);

    }
    TEST_F(FastConv, TimeDomainFlushBufferTest)
    {
       // float TestImpulse[51] = { 0 };
        float TestInput[10] = { 0 };
        float TestOutput[10] = { 0 };
        float TestFlush[50] = { 0 };
        float CheckOutput[60] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
       //     TestImpulse[i] = static_cast<float>(std::rand()) / (static_cast <float> (RAND_MAX));
         //   TestImpulse[i] = TestImpulse[i] * 2.0 - 1.0;
            CheckOutput[i + 3] = TestImpulse[i];
        }


        m_pCFastConv->init(TestImpulse, 51, 1024, CFastConv::kTimeDomain);
        m_pCFastConv->process(TestOutput, TestInput, 10);
        m_pCFastConv->flushBuffer(TestFlush);

        CHECK_ARRAY_CLOSE(TestOutput, CheckOutput, 10, 1e-3);
        CHECK_ARRAY_CLOSE(TestFlush, TestImpulse + 7, 51 - 7, 1e-3);

    }

    TEST_F(FastConv, TimeDomainBlockSizeTest)
    {
      //  float TestImpulse[51] = { 0 };
        float TestInput[10000] = { 0 };
        float TestOutput[10000] = { 0 };
        int BufferSize[8] = { 1, 13, 1023, 2048, 1, 17, 5000, 1897 };
        int InputStartIdx[8] = { 0 }; // All of the buffersizes add up to 10000, so we can just start the reading of the input at shifted positions


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

    ////////////////////////////////////////////////////
    // FREQ DOMAIN TESTS
    ////////////////////////////////////////////////////

    TEST_F(FastConv, FreqDomainIdentityTest)
    {
        //float TestImpulse[51] = { 0 };
        float TestInput[128] = { 0 };
        float TestOutput[128] = { 0 };
        float CheckOutput[60] = { 0 };
        TestInput[20] = 1;
        for (int i = 0; i < 51; i++)
        {
            if (i < (51 - 23)) {
                CheckOutput[i + 3 + 20] = TestImpulse[i];
            }
        }


        m_pCFastConv->init(TestImpulse, 128, 64, CFastConv::kFreqDomain);

        m_pCFastConv->process(TestOutput, TestInput, 128);
        CHECK_ARRAY_CLOSE(CheckOutput, TestOutput, 51, 1e-3);

    }

    TEST_F(FastConv, FreqDomainFlushIdentifyTest)
    {
        float TestInput[128] = { 0 };
        float TestOutput[128] = { 0 };
        float TestFlush[82] = { 0 };
        TestInput[3] = 1;
        for (int i = 0; i < 51; i++)
        {
            TestImpulse[i] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        }

        m_pCFastConv->init(TestImpulse, 51, 32, CFastConv::kFreqDomain);
        m_pCFastConv->process(TestOutput, TestInput, 128);
        m_pCFastConv->flushBuffer(TestFlush);

        CHECK_ARRAY_CLOSE(TestFlush + 3 + 32 - 10, TestImpulse, 51, 1e-3);
    }
//
    TEST_F(FastConv, FreqDomainDifferBlockSize)
    {
        float* TestImpulse =  new float[16384];
        CVectorFloat::setZero(TestImpulse,16384);
        float* TestInput = new float[10000];
        CVectorFloat::setZero(TestInput,10000);
        float* TestOutput = new float[10000];
        CVectorFloat::setZero(TestOutput,10000);
        int Impulse_blockSize = 1024;
        //(L-1 + Initial delay)
        float* TestFlush = new float[16383 + Impulse_blockSize];
        long long int fullSize = 16383 + Impulse_blockSize;

        int BlockSizes[8] = { 1, 13, 1023, 2048, 1, 17, 5000, 1897 };
        int StartIdx[8] = { 0 };
        for (int i = 1; i < 8; i++)
        {
            StartIdx[i] = StartIdx[i - 1] + BlockSizes[i - 1];
        }
        TestInput[3] = 1;
        for (int i = 0; i < 16384; i++)
        {
            TestImpulse[i] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        }
        m_pCFastConv->init(TestImpulse, 16384, 1024, CFastConv::kFreqDomain);

        for (int i = 0; i < 8; i++)
        {
            m_pCFastConv->process(TestOutput + StartIdx[i], TestInput + StartIdx[i], BlockSizes[i]);
            CHECK_ARRAY_CLOSE(TestOutput + 3 + Impulse_blockSize + StartIdx[i], TestImpulse, BlockSizes[i] - 3 - Impulse_blockSize, 1e-3);
        }

        m_pCFastConv->flushBuffer(TestFlush);
        CHECK_ARRAY_CLOSE(TestFlush + Impulse_blockSize, TestImpulse + 10000 - 3, 16384 - (10000 - 3), 1e-3);

        delete[] TestImpulse;
        delete[] TestInput;
        delete[] TestOutput;
        delete[] TestFlush;

    }
//
//
//
//
}

#endif //WITH_TESTS

