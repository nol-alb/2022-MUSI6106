
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"
#include "ErrorDef.h"
#include <string>

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
int test1();
int test2();
int test3();
int test4();
int test5();

/////////////////////////////////////////////////////////////////////////////////
// main function
//
//  BasicTest.cpp
//  MUSI6106Exec
//
//  Created by Noel  Alben on 2/12/22.
//


/*
 [30] Add tests to your main function
 If your main is called without command line arguments, automatically run tests verifying your implementation. Implement the following test (a function each):
 FIR: Output is zero if input freq matches feedforward
 IIR: amount of magnitude increase/decrease if input freq matches feedback
 FIR/IIR: correct result for VARYING input block size
 FIR/IIR: correct processing for zero input signal
 One more additional MEANINGFUL test to verify your filter implementation
 */
using std::cout;
using std::endl;
int Basic_RunTest()
{
    std::cout << "---------------------"<< std::endl;
    std::cout << "-----Sanity TEST-----"<< std::endl;
    std::cout << "---------------------"<< std::endl;




    static const int kBlockSize = 1024;

    clock_t time = 0;

    CAudioFileIf *phOutputAudioFile = nullptr;
    CAudioFileIf *phInputAudioFile=nullptr;
    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;

    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/6.wav";
    std::string sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/6_Filters.wav";
    CAudioFileIf::FileSpec_t stFileSpec;


    float delayTimeInSec=0.25;
    float gain=1;

    CCombFilterIf *p_Combfilter;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;


    // Creating the Audio Files buffer in and out

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputAudioFile->isOpen()) {
        std::cout << "Output wav file open error!";
        return -1;
    }


    ppfInputAudioData = new float *[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    //Create the CombFilters
    CCombFilterIf::create(p_Combfilter);
    p_Combfilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    p_Combfilter->setParam(CCombFilterIf::kParamGain, gain);
    p_Combfilter->setParam(CCombFilterIf::kParamDelay, delayTimeInSec);

    //Read Audio Write AUDIO that is filtered
    long long int BlockSize = kBlockSize;
    while(!phInputAudioFile->isEof())
    {
        phInputAudioFile->readData(ppfInputAudioData, BlockSize);
        p_Combfilter->process(ppfInputAudioData, ppfOutputAudioData, BlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, BlockSize);
    }

    std::cout<<"Reading and Writing done, with a filter";
    std::cout << "Output: " << sOutputFilePath << std::endl;
    // Clean-up
    phInputAudioFile->closeFile();
    phOutputAudioFile->closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;

    std::cout<<"Everything Basically Works"<<endl;

}


int main(int argc, char* argv[])
{
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    static const int kBlockSize = 1024;
    float delayTimeInSec;
    float gain;

    clock_t time = 0;

    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData=nullptr;
    CAudioFileIf *phInputAudioFile = nullptr;
    CAudioFileIf *phOutputAudioFile = nullptr;
    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf *pCombFilter = nullptr;
    CCombFilterIf::CombFilterType_t filterType;
    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    //Basic_RunTest();
    // parse command line arguments
    if (argc==1) {
        std::cout << "Testing Testing" << std::endl;
        Basic_RunTest();
        test1();
        test2();
        test3();
        test4();
        test5();

    }
    if (argc > 1) {
        sInputFilePath = argv[1];
    }

    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else {
        std::cout << "Missing: Input audio path and the Filter type ('FIR' or 'IIR')" << std::endl;
        return -1;
    }
    if (argc > 2) {
        std::string arg = argv[2];
        if (arg == "FIR")
            filterType = CCombFilterIf::kCombFIR;
        else if (arg == "IIR")
            filterType = CCombFilterIf::kCombIIR;
        else {
            std::cout << "Missing:Filter type ('FIR' or 'IIR')" << std::endl;
            return -1;
        }
        sOutputFilePath = sInputFilePath.substr(0, sInputFilePath.size() - 4).append("_FilteredIn_" + arg + "_cpp.wav");
    }
    if (argc > 3) {
        delayTimeInSec = atof(argv[3]);
        if (delayTimeInSec < 0)
            std::cout << "Delay time cannot be negative. Will be set to 0." << std::endl;
    }
    else
        delayTimeInSec = 0.01;
    if (argc > 4) {
        gain = atof(argv[4]);
        if (gain > 1)
            std::cout << "Gain cannot be > 1. Will be set to 1." << std::endl;
        else if (gain < -1)
            std::cout << "Gain cannot be < -1. Will be set to -1." << std::endl;
    }
    else
        gain = 0.5;

    std::cout << "Filter Type: " << argv[2] << std::endl;
    std::cout << "Delay (sec): " << delayTimeInSec << std::endl;
    std::cout << "Gain: " << gain << std::endl;

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(phInputAudioFile);
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    // open the Output wave file
    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite);
    if (!phOutputAudioFile->isOpen())
    {
        cout << "Output Wave file open error!";
        CAudioFileIf::destroy(phOutputAudioFile);
        return -1;
    }


    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfInputAudioData = new float*[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float*[stFileSpec.iNumChannels];

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i]= new float[kBlockSize];
    }

    /////////////////////////////////////////////////////////////////////////////
    // CREATE THE COMBFILTERS
    CCombFilterIf::create(pCombFilter);
    pCombFilter->init(filterType,delayTimeInSec,stFileSpec.fSampleRateInHz,stFileSpec.iNumChannels);
    pCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, gain );
    pCombFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, delayTimeInSec );

    long long BlockSize = kBlockSize;
    while (!phInputAudioFile->isEof()) {
        phInputAudioFile->readData(ppfInputAudioData, BlockSize);
        pCombFilter->process(ppfInputAudioData, ppfOutputAudioData, (int)BlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, BlockSize);
    }
    cout << "Reading and writing done in: \t" << endl;

    ////////////////////////////////////////////////////////////////////////////////
    //Clean-Up
    CCombFilterIf::destroy(pCombFilter);
    phInputAudioFile->closeFile();
    phOutputAudioFile->closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;



    // all done
    return 0;

}

int test1()
{
    static const int kBlockSize = 1024;

    std::cout << "-------------------------------"<< std::endl;
    std::cout << "--------TEST 1 (FIR = 0)-------"<< std::endl;
    std::cout << "-------------------------------"<< std::endl;
    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/Sine_Tone100.wav";
    float delayTimeInSec = 0.01; //441 samples is one cycle, so 44100/100 ~ 1/100 ~ -.01
    float gain = 1;
    CAudioFileIf *phOutputAudioFile = nullptr;
    CAudioFileIf *phInputAudioFile=nullptr;
    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;

    std::string sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/Sine_Tone100_FILTERED_Zero.wav";
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf *p_Combfilter;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;


    // Creating the Audio Files buffer in and out

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputAudioFile->isOpen()) {
        std::cout << "Output wav file open error!";
        return -1;
    }


    ppfInputAudioData = new float *[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    //Create the CombFilters
    CCombFilterIf::create(p_Combfilter);
    p_Combfilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    p_Combfilter->setParam(CCombFilterIf::kParamGain, gain);
    p_Combfilter->setParam(CCombFilterIf::kParamDelay, delayTimeInSec);

    //Read Audio Write AUDIO that is filtered
    long long int BlockSize = kBlockSize;
    while(!phInputAudioFile->isEof())
    {
        phInputAudioFile->readData(ppfInputAudioData, BlockSize);
        p_Combfilter->process(ppfInputAudioData, ppfOutputAudioData, BlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, BlockSize);
    }

    std::cout<<"Reading and Writing done, with a filter";
    std::cout << "Output: " << sOutputFilePath << std::endl;
    // Clean-up
    phInputAudioFile->closeFile();
    phOutputAudioFile->closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;
    std::cout << "---------------" << std::endl;
    std::cout << "Test 1 completed" << std::endl;
    std::cout << "---------------" << std::endl;



    return 0;


}
int test2()
{
    std::cout << "-------------------------------"<< std::endl;
    std::cout << "--------TEST 1 (IIR ><)--------"<< std::endl;
    std::cout << "-------------------------------"<< std::endl;
    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/Sine_Tone100.wav";
    std::string sOutputFilePath;
    float delayTimeInSec = 0.01; //delay by one period
    float All_gain[] = {1,-1,0.2};

    //Multiple BlockSizes
    long long int blockSize = 1024;
    float **ppfInputAudioData = 0;
    float **ppfOutputAudioData = 0;

    CAudioFileIf *phInputAudioFile = 0;
    CAudioFileIf *phOutputAudioFile = 0;

    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf *pCombFilter = 0;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombIIR;


    // Open the input and create output text file
    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);


    // https://stackoverflow.com/questions/41687921/looping-through-array-using-pointers

    for (long long int gain : All_gain) {

        std::cout << "Test IIR with Gain = " << gain << std::endl;
        std::string gain_str = std::to_string(gain);
        sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/Output_Sine_Tone100" + gain_str+ ".wav";
        CAudioFileIf::create(phOutputAudioFile);
        phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
        if (!phOutputAudioFile->isOpen()) {
            std::cout << "Output wav file open error!";
            return -1;
        }
        phInputAudioFile->setPosition((long long) 0);

        // Allocate memory for audio i/o buffers
        ppfInputAudioData = new float *[stFileSpec.iNumChannels];
        ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            ppfInputAudioData[i] = new float[blockSize];
            ppfOutputAudioData[i] = new float[blockSize];
        }

        // Create Comb Filter
        CCombFilterIf::create(pCombFilter);
        pCombFilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
        pCombFilter->setParam(CCombFilterIf::kParamGain, gain);

        // Read, filter and write audio

        while (!phInputAudioFile->isEof()) {

            phInputAudioFile->readData(ppfInputAudioData, blockSize);
            pCombFilter->process(ppfInputAudioData, ppfOutputAudioData, (int)blockSize);
            phOutputAudioFile->writeData(ppfOutputAudioData, blockSize);
        }
        std::cout << "Reading and writing done in: \t" << std::endl;
        std::cout << "Output: " << sOutputFilePath << std::endl;

        // Clean up
        CCombFilterIf::destroy(pCombFilter);

        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            delete[] ppfInputAudioData[i];
            delete[] ppfOutputAudioData[i];
        }
        delete[] ppfInputAudioData;
        delete[] ppfOutputAudioData;
        ppfInputAudioData = nullptr;
        ppfOutputAudioData = nullptr;

        phOutputAudioFile->reset();
    }

    phInputAudioFile->reset();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    std::cout << "---------------" << std::endl;
    std::cout << "Test 2 completed" << std::endl;
    std::cout << "---------------" << std::endl;
    return 0;
}

int test3()
{
    // Use a pure tone to implement this test sine 100 hz

    std::cout << "---------------------"<< std::endl;
    std::cout << "--------TEST 3-------"<< std::endl;
    std::cout << "---------------------"<< std::endl;
    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/Sine_Tone100.wav";
    std::string sOutputFilePath;
    float delayTimeInSec = 0.01; //delay by one period
    float gain = 0.5;

    //Multiple BlockSizes
    long long int blockSizes[] = {1024, 512, 256};
    float **ppfInputAudioData = 0;
    float **ppfOutputAudioData = 0;

    CAudioFileIf *phInputAudioFile = 0;
    CAudioFileIf *phOutputAudioFile = 0;

    CAudioFileIf::FileSpec_t stFileSpec;

    CCombFilterIf *pCombFilter = 0;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;


    // Open the input and create output text file
    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);


    // https://stackoverflow.com/questions/41687921/looping-through-array-using-pointers

    for (long long int blockSize : blockSizes) {

        std::cout << "Test with blockSize = " << blockSize << std::endl;
        std::string Block_str = std::to_string(blockSize);
        sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/Output_Sine_Tone100" + Block_str+ ".wav";
        CAudioFileIf::create(phOutputAudioFile);
        phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
        if (!phOutputAudioFile->isOpen()) {
            std::cout << "Output wav file open error!";
            return -1;
        }
        phInputAudioFile->setPosition((long long) 0);

        // Allocate memory for audio i/o buffers
        ppfInputAudioData = new float *[stFileSpec.iNumChannels];
        ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            ppfInputAudioData[i] = new float[blockSize];
            ppfOutputAudioData[i] = new float[blockSize];
        }

        // Create Comb Filter
        CCombFilterIf::create(pCombFilter);
        pCombFilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
        pCombFilter->setParam(CCombFilterIf::kParamGain, gain);

        // Read, filter and write audio

        while (!phInputAudioFile->isEof()) {

            phInputAudioFile->readData(ppfInputAudioData, blockSize);
            pCombFilter->process(ppfInputAudioData, ppfOutputAudioData, (int)blockSize);
            phOutputAudioFile->writeData(ppfOutputAudioData, blockSize);
        }
        std::cout << "Reading and writing done in: \t" << std::endl;
        std::cout << "Output: " << sOutputFilePath << std::endl;

        // Clean up
        CCombFilterIf::destroy(pCombFilter);

        for (int i = 0; i < stFileSpec.iNumChannels; i++) {
            delete[] ppfInputAudioData[i];
            delete[] ppfOutputAudioData[i];
        }
        delete[] ppfInputAudioData;
        delete[] ppfOutputAudioData;
        ppfInputAudioData = nullptr;
        ppfOutputAudioData = nullptr;

        phOutputAudioFile->reset();
    }

    phInputAudioFile->reset();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    std::cout << "---------------" << std::endl;
    std::cout << "Test 3 completed" << std::endl;
    std::cout << "---------------" << std::endl;
    return 0;
}





int test4()
{
    static const int kBlockSize = 1024;

    std::cout << "-----------------------------"<< std::endl;
    std::cout << "--------TEST 4 (Empty)-------"<< std::endl;
    std::cout << "-----------------------------"<< std::endl;
    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/Empty.wav";
    float delayTimeInSec = 0.01;
    float gain = 0.5;
    CAudioFileIf *phOutputAudioFile = nullptr;
    CAudioFileIf *phInputAudioFile=nullptr;
    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;

    std::string sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/Empty_filtered.wav";
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf *p_Combfilter;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;


    // Creating the Audio Files buffer in and out

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputAudioFile->isOpen()) {
        std::cout << "Output wav file open error!";
        return -1;
    }


    ppfInputAudioData = new float *[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    //Create the CombFilters
    CCombFilterIf::create(p_Combfilter);
    p_Combfilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    p_Combfilter->setParam(CCombFilterIf::kParamGain, gain);
    p_Combfilter->setParam(CCombFilterIf::kParamDelay, delayTimeInSec);

    //Read Audio Write AUDIO that is filtered
    long long int BlockSize = kBlockSize;
    while(!phInputAudioFile->isEof())
    {
        phInputAudioFile->readData(ppfInputAudioData, BlockSize);
        p_Combfilter->process(ppfInputAudioData, ppfOutputAudioData, BlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, BlockSize);
    }

    std::cout<<"Reading and Writing done, with a filter";
    std::cout << "Output: " << sOutputFilePath << std::endl;
    // Clean-up
    phInputAudioFile->closeFile();
    phOutputAudioFile->closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;
    std::cout << "---------------" << std::endl;
    std::cout << "Test 4 completed" << std::endl;
    std::cout << "---------------" << std::endl;



    return 0;


}
int test5()
{
    std::cout << "--------------------------------------"<< std::endl;
    std::cout << "--------TEST 5 (Illegal Values)-------"<< std::endl;
    std::cout << "--------------------------------------"<< std::endl;
    static const int kBlockSize = 1024;
    std::string sInputFilePath = "/Users/noelalben/Desktop/AUDIOS/6.wav";
    float delayTimeInSec = -23;
    float gain = -5;
    CAudioFileIf *phOutputAudioFile = nullptr;
    CAudioFileIf *phInputAudioFile=nullptr;
    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;

    std::string sOutputFilePath = "/Users/noelalben/Desktop/AUDIOS/6_filtered.wav";
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf *p_Combfilter;
    CCombFilterIf::CombFilterType_t filterType = CCombFilterIf::kCombFIR;


    // Creating the Audio Files buffer in and out

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen()) {
        std::cout << "Input wav file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputAudioFile->isOpen()) {
        std::cout << "Output wav file open error!";
        return -1;
    }


    ppfInputAudioData = new float *[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float *[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    //Create the CombFilters
    CCombFilterIf::create(p_Combfilter);
    p_Combfilter->init(filterType, delayTimeInSec, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    p_Combfilter->setParam(CCombFilterIf::kParamGain, gain);
    p_Combfilter->setParam(CCombFilterIf::kParamDelay, delayTimeInSec);

    //Read Audio Write AUDIO that is filtered
    long long int BlockSize = kBlockSize;
    while(!phInputAudioFile->isEof())
    {
        phInputAudioFile->readData(ppfInputAudioData, BlockSize);
        p_Combfilter->process(ppfInputAudioData, ppfOutputAudioData, BlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, BlockSize);
    }

    std::cout<<"Reading and Writing done, with a filter";
    std::cout << "Output: " << sOutputFilePath << std::endl;
    // Clean-up
    phInputAudioFile->closeFile();
    phOutputAudioFile->closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for (int i = 0; i < stFileSpec.iNumChannels; i++) {
        delete[] ppfInputAudioData[i];
        delete[] ppfOutputAudioData[i];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;
    std::cout << "---------------" << std::endl;
    std::cout << "Test 5 completed" << std::endl;
    std::cout << "---------------" << std::endl;



    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}

