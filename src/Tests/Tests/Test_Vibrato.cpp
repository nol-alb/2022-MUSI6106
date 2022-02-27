#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Vector.h"
#include "Vibrato.h"
#include "AudioFileIf.h"

#include "gtest/gtest.h"


namespace vibrato_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
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
        // You can do clean-up work that doesn't throw exceptions here.
        CVibrato::destroy(p_CVibratoTest);
        for (int i = 0; i <iNumChannels; i++)
        {
            delete [] InputTestBuff[i];
            delete [] OutputTestBuff[i];
        }
        delete [] InputTestBuff;
        delete [] OutputTestBuff;
        InputTestBuff = 0;
        OutputTestBuff = 0;
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

TEST_F(CVibratoTests, SetParams){
    EXPECT_EQ(p_CVibratoTest->kWidthInSec,fTestWidth);


}



//int main(int argc, char* argv[]) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
#endif //WITH_TESTS
