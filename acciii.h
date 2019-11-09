#include <unistd.h>
#include <QApplication>
#include "DataProc.h"
#include<iostream>
//#include<thread>

class AccIII
{
public:
    explicit AccIII(int argc, char** argv);
    ~AccIII();

    void transmitData();
    int printData();

private:
    DWORD EventDWord;
    DWORD RxBytes;
    DWORD TxBytes;

    long dwSum;
    unsigned char* fileBuffer;  //data form FPGA stored in this buffer
    int DataNum;

    //HANDLE ftHandle;
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    UCHAR Mask;
    UCHAR Mode;
    UCHAR LatencyTimer; //our default setting is 16

    DWORD BytesWritten;
    char TxBuffer[1];

    //Data transmission parameters
    LARGE_INTEGER lPreTime, lPostTime, lFrequency;
    DWORD BytesReceivedTest;
    DWORD readBytesTest = 0;
    unsigned char RxBuffer[10000];

    void setupUSB(int argc, char** argv);

};
