#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Synthesis.h"

#include "Vector.h"
#include "FastConv.h"
#include "RingBuffer.h"

#include "gtest/gtest.h"

//TODO: [15] Implement three tests (in Test_FastConv.cpp)
// identity: generate a random IR of length 51 samples and an impulse as input signal at sample index 3; make the input signal
// 10 samples long. Check the correct values of the 10 samples of the output signal.
// flushbuffer: use the same signals as in identity, but now get the full tail of the impulse response and check for correctness
// blocksize: for an input signal of length 10000 samples, implement a test similar to identity with a succession of different
// input/output block sizes (1, 13, 1023, 2048,1,17, 5000, 1897)

//TODO: Duplicate your above tests so they now verify computation in the frequency domain and compensate appropriately for
// latency at different blocksizes.
//TODO: [6] Build your program in release mode and use <ctime>.
// Compare the runtime performance between your time domain and frequency domain implementations in a markdown file.



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
            CFastConv m_pCFastConv;

            // why do I have a linker error with CRingBuffer
            



        }

        virtual void TearDown()
        {
        }

        CFastConv *m_pCFastConv = 0;
    };

    TEST_F(FastConv, TimeDomainConv)
    {
        float x[] = { 1.f, 1.f, 1.f };
        float h[] = {1.f, 2.f, 0.f, -1.f};
        
        float *outputBuf = new float[11];
        float expectedOutputBuf[] = { 1.f,3.f,3.f,-1.f,-3.f,-3.f};



        m_pCFastConv->init(x, 3, 8192, CFastConv::ConvCompMode_t::kTimeDomain);
        m_pCFastConv->process(outputBuf, x, 3);

        
        
        //CFastConv::init(float* pfImpulseResponse, int iLengthOfIr, int iBlockLength /*= 8192*/, ConvCompMode_t eCompMode /*= kFreqDomain*/);
        
        for (int i = 0; i < sizeof(expectedOutputBuf) / sizeof(expectedOutputBuf[0]); i++) {
            EXPECT_NEAR(outputBuf[i], expectedOutputBuf[i], .001);
        }


    }
}

#endif //WITH_TESTS

