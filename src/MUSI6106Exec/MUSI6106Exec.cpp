
#include <iostream>
#include <ctime>
#include <cstring>
#include <algorithm>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Fft.h"
#include "RingBuffer.h"
#include "Vector.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string         InputFilepath,
                        OutputFilepath;
    static const int    kBlockSize = 1024;
    clock_t             time = 0;
    float               **ppfInputAudioBuffer=nullptr;
    float               **additionalBuffer=nullptr;
    CRingBuffer<float>* m_ptBuff=0;

    CFft::complex_t     *pfSpectralData=nullptr;
    CFft                *pFft;
    float               *pfMag;
    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;
    int                     fftBlockSize;
    int                     fftHopSize;
    showClInfo();
    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    InputFilepath = argv[1];
    OutputFilepath = InputFilepath.substr(0, InputFilepath.size() - 4).append("_stft.txt");
    fftBlockSize = atoi(argv[2]);
    fftHopSize = atoi(argv[3]);
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file

    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(InputFilepath, CAudioFileIf::FileIoType_t::kFileRead, &stFileSpec);
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////

    hOutputFile.open(OutputFilepath, std::fstream::out);
    hOutputFile.precision(16);
    //////////////////////////////////////////////////////////////////////////////

    ppfInputAudioBuffer = new float*[stFileSpec.iNumChannels];
    float* fftBuffer = 0;
    fftBuffer = new float(fftBlockSize);
    int fullsize = fftBlockSize+fftHopSize;
    for (int i = 0; i <stFileSpec.iNumChannels;i++)
    {
        ppfInputAudioBuffer[i] = new float(fullsize);

    }


    pfSpectralData = new float[fftBlockSize];
    pfMag = new float [int(fftBlockSize / 2 + 1)];
    CFft::createInstance(pFft);
    pFft->initInstance(fftBlockSize);

    long long hopSize = fftHopSize;
    long long fullSize = fftBlockSize+fftHopSize;
    long long fftMagSize = int(fftBlockSize/2 + 1);

    while(!phAudioFile->isEof()) {
        CVectorFloat::setValue(ppfInputAudioBuffer[0],0,fullsize);
        phAudioFile->readData(ppfInputAudioBuffer,fullSize);
        for (int i = 0; i<2; i++)
        {
            CVector::copy(fftBuffer, ppfInputAudioBuffer[0], fftBlockSize);
            pFft->doFft(pfSpectralData,fftBuffer);
            pFft->getMagnitude(pfMag,pfSpectralData);
            CVectorFloat::moveInMem(ppfInputAudioBuffer[0], 0, fftHopSize, fftBlockSize);
            for (long long i = 0; i < fftMagSize; i++) {
                hOutputFile << pfMag[i] << "\t";
            }
            hOutputFile << std::endl;
        }



    }



    ///////////////////////////////////////////////////////////////////////////////
    //Clean Up
    for (int i=0; i<stFileSpec.iNumChannels;i++)
    {
        delete[] ppfInputAudioBuffer[i];

    }
    delete ppfInputAudioBuffer;
    ppfInputAudioBuffer=0;

    delete[] fftBuffer;
    fftBuffer=0;

    delete[] pfSpectralData;
    pfSpectralData=0;
    delete[] pfMag;
    pfMag=0;
    CFft::destroyInstance(pFft);
    phAudioFile->closeFile();
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();


    return 0;


}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout << endl;

    return;
}

