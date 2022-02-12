
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
//
//  BasicTest.cpp
//  MUSI6106Exec
//
//  Created by Noel  Alben on 2/12/22.
//

#include <stdio.h>
#include<iostream>
#include "MUSI6106Config.h"
#include "AudioFileIf.h"
#include "CombFilterIf.h"
#include "ErrorDef.h"

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

    static const int kBlockSize = 1024;

    clock_t time = 0;

    CAudioFileIf *phOutputAudioFile = 0;
    CAudioFileIf *phInputAudioFile=0;
    float **ppfInputAudioData = 0;
    float **ppfOutputAudioData = 0;

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

    std::cout<<"Everything Basically Works";

}


int main(int argc, char* argv[])
{
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    static const int kBlockSize = 1024;

    clock_t time = 0;

    float **ppfAudioData = 0;

    CAudioFileIf *phAudioFile = 0;
    std::fstream hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    Basic_RunTest();
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(phAudioFile);
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        CAudioFileIf::destroy(phAudioFile);
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];

    if (ppfAudioData == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }
    if (ppfAudioData[0] == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }

    time = clock();

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    while (!phAudioFile->isEof())
    {
        // set block length variable
        long long iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phAudioFile->readData(ppfAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        // write
        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioData[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

