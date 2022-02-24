
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "RingBuffer.h"
#include "Fft.h"
#include <complex>


using std::cout;
using std::endl;

// local function declarations
void    showClInfo();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
        sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float** ppfAudioData = 0;
    int fft_blockSize;
    int fft_hopSize;

    CAudioFileIf* phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    CRingBuffer<float>  *m_ppCRingBuffer;
    CFft* Fft =0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        fft_blockSize = atoi(argv[2]);
        fft_hopSize = atoi(argv[3]);
        sOutputFilePath = sInputFilePath + ".txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    int bufferSize = fft_blockSize+fft_hopSize;
    m_ppCRingBuffer = new CRingBuffer<float>(bufferSize);

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
    ppfAudioData = new float* [stFileSpec.iNumChannels];
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

    CFft::createInstance(Fft);
    Fft->initInstance(kBlockSize,fft_blockSize);
    CFft::complex_t* fftOutputBuffer;
    fftOutputBuffer = new CFft::complex_t (fft_blockSize);


    float* processblock;
    processblock = new float(fft_blockSize);



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
                m_ppCRingBuffer->putPostInc(ppfAudioData[c][i]);
                int write_now = m_ppCRingBuffer->getWriteIdx();
                int total_length = m_ppCRingBuffer->getLength();
                if(write_now == total_length)
                {
                    for (int k= 0; k<fft_blockSize;k++)
                    {
                        processblock[k] = m_ppCRingBuffer->getPostInc();
                    }
                    int read_now = m_ppCRingBuffer->getReadIdx();
                    int hopped_idx = read_now+fft_hopSize;
                    m_ppCRingBuffer->setReadIdx(hopped_idx);
                    Fft->doFft(fftOutputBuffer,processblock);
                    hOutputFile << fftOutputBuffer << "\t";
                }
                else
                {
                    int zero_add = total_length-write_now;
                    for (int k = 0; k<zero_add;k++)
                        m_ppCRingBuffer->putPostInc(0);
                    for (int k= 0; k<fft_blockSize;k++)
                    {
                        processblock[k] = m_ppCRingBuffer->getPostInc();
                    }
                    int read_now = m_ppCRingBuffer->getReadIdx();
                    int hopped_idx = read_now+fft_hopSize;
                    m_ppCRingBuffer->setReadIdx(hopped_idx);
                    Fft->doFft(fftOutputBuffer,processblock);
                    hOutputFile << fftOutputBuffer << "\t";

                }

            }
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();
    CFft::destroyInstance(Fft);
    delete m_ppCRingBuffer;
    m_ppCRingBuffer=0;


    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    delete processblock;
    processblock=0;

    delete fftOutputBuffer;
    fftOutputBuffer=0;

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout << endl;

    return;
}

