#include <unistd.h>
#include <QApplication>
#include "DataProc.h"
#include<iostream>
#include<cstring>
#include<vector>
//#include<thread>

class AccIII
{
public:
    explicit AccIII(int argc, char** argv);
    ~AccIII();

    void transmitData();
    void setSamplingTime(float time);
    std::vector<std::vector<float>> printData();
    int printDataSetNum();

private:
    DWORD EventDWord;
    DWORD RxBytes;
    DWORD TxBytes;

    long dwSum;
    unsigned char* fileBuffer;  //data form FPGA stored in this buffer
    int DataNum;
    float samp_time;  //sampling time in second, default is 10.0
	
	//Decoding configuration parameters and variables 
    const float GSCALE = 0.00073; // Unit coversion (0.73 mg/digit)
	unsigned int READNUM = 46;
	unsigned int HALFREAD = 23;
	
    std::vector<std::vector<float>> decoded_data;           // decoded_data is a vector of size dataSetNum * 138, 138 = 3 * 46

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
    unsigned char RxBuffer[10000];

    //Number of data set acquired
    int dataSetNum;     // data buffer has size dataSetNum*138

    void setupUSB(int argc, char** argv);

};
