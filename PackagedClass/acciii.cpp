#include "acciii.h"

Acciii::Acciii() : GSCALE(0.00073), READNUM(10), HALFREAD((int)(READNUM * 0.5)), sampleTime(1.0)
{
    dwSum = 0;
    fileBuffer = nullptr;


    DataNum = sampleTime * ExpFs * AccBusNum * DataByteNum;
    fileBuffer = new unsigned char[DataNum];

}

Acciii::~Acciii(){
    free(fileBuffer); // Free buffer memory to avoid memory leak

}

void Acciii::sampleData(){
    // ------------------------------------------------------
    UCHAR Mask = 0xff;
    UCHAR Mode;

    UCHAR LatencyTimer = 2; //our default setting is 16

    DWORD BytesWritten;
    char TxBuffer[1] = { 0x55 };

    ftStatus = FT_Open(0, &ftHandle);
    idDataRate = 0.0; // sampling rate;

    if (ftStatus != FT_OK)
    {
        printf("FT_Open FAILED!\r\n");
    }
    else
    {
        printf("FT_Open Succeeded!\r\n");
    }

    Mode = 0x00; //reset mode
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);

    if (ftStatus == FT_OK)
    {
        ftStatus = FT_SetLatencyTimer(ftHandle, LatencyTimer);
        if (ftStatus == FT_OK) {
            printf("FT_SetLatencyTimer Succeeded!\r\n");
        }
        else {
            printf("FT_SetLatencyTimer FAILED!\r\n");
        }

        ftStatus = FT_SetUSBParameters(ftHandle, 0x10000, 0x1);
        if (ftStatus == FT_OK) {
            printf("FT_SetUSBParameters Succeeded!\r\n");
        }
        else {
            printf("FT_SetUSBParameters FAILED!\r\n");
        }

        ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0);
        if (ftStatus == FT_OK) {
            printf("FT_SetFlowControl Succeeded!\r\n");
        }
        else {
            printf("FT_SetFlowControl FAILED!\r\n");
        }

        ftStatus = FT_Purge(ftHandle, FT_PURGE_RX);
        if (ftStatus == FT_OK) {
            printf("FT_Purge Succeeded!\r\n");
        }
        else {
            printf("FT_Purge FAILED!\r\n");
        }

        ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer), &BytesWritten);
        if (ftStatus == FT_OK) {
            printf("FT_Write Succeeded!\r\n");
        }
        else {
            printf("FT_Write Failed\r\n");
        }

        // Setup finished
        printf("Ready to sample\r\n");

        uint64_t lPreTime, lPostTime;
        lPreTime = GetPIDTimeInNanoseconds();

        dwSum = 0;

        printf("Sampling...\n");
        while (dwSum < DataNum)
        {
            ftStatus = FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);
            //printf("ftStatus = %d, RxBytes = %d\n", ftStatus, RxBytes);

            if ((ftStatus == FT_OK) && (RxBytes > 0))
            {
                if (RxBytes < 10000)
                {
                    USBReadData(ftHandle, RxBytes, &dwSum, DataNum, fileBuffer);
                }
                else
                {
                    int iCount = RxBytes / 10000;
                    for (int i = 0; i < iCount; i++)
                    {
                        USBReadData(ftHandle, 10000, &dwSum, DataNum, fileBuffer);
                    }

                    int iMod = RxBytes % 10000;
                    if (iMod > 0)
                    {
                        USBReadData(ftHandle, iMod, &dwSum, DataNum, fileBuffer);
                    }
                }
            }
        }

        //QueryPerformanceCounter(&lPostTime);
        lPostTime = GetPIDTimeInNanoseconds();
        float lPassTime = (lPostTime - lPreTime) * 0.000000001;


        FT_Close(ftHandle);
        printf("Begin to save data into file!\r\n");


        idDataRate = dwSum / (lPassTime * 6 * READNUM); // Count ID as data

        /*
        //SaveDataResult(dwSum, fileBuffer);
        printf("File Save Done!\r\n");

        //SaveNum(lPassTime, "sample_time.txt");

        SaveNum(idDataRate, "data_rate.txt");
        */

    }
    else
    {
        // FT_SetBitMode FAILED!
        FT_Close(ftHandle);
    }

}

std::vector<std::vector<double>> Acciii::decodeData(){
    std::vector<std::vector<double>> decoded_data;

    // Process and decode USB data
    long int read_byte_num = dwSum;
    long int byte_per_sensor = (read_byte_num / READNUM) -1;
    long int samp_num = (byte_per_sensor / 6);
    int start_i = READNUM+1; // Skip the beginning WHO_AM_I check values
    //int end_i = start_i + (byte_per_sensor*READNUM) -1

    unsigned char* hex_data = new unsigned char[byte_per_sensor*READNUM];
    memcpy(hex_data,(fileBuffer+start_i),byte_per_sensor*READNUM);

    long int hex_i = 0;
    for (int i = 0; i < samp_num; ++i) // One sample contains 46 sensor data
    {
        std::vector<double> a_sample(3*READNUM); // [Acc0_X,Acc0_Y,Acc0_Z,Acc1_X,...,Acc46_Z] 46*3 = 138

        // Odd number sensor (Pull up I2C)
        for (int j = 0; j < 3; ++j) // Axis X, Y, Z
        {
            for (int k = 0; k < HALFREAD; ++k) // Odd number Sensor 1,2,...,23
            {
                a_sample[6*k+j] = (((int)hex_data[hex_i+k+HALFREAD]&0xFF)<<8) | ((int)hex_data[hex_i+k]&0xFF); //[High,Low]

                if (a_sample[6*k+j]> 32767) a_sample[6*k+j] -= 65536; // # Format correction

                a_sample[6*k+j] *= GSCALE; // Unit converted to g (gravity)
            }
            hex_i += READNUM;
        }

        // Even number sensor (Pull down I2C)
        for (int j = 0; j < 3; ++j) // Axis X, Y, Z
        {
            for (int k = 0; k < HALFREAD; ++k) // Even number Sensor 1,2,...,23
            {
                a_sample[6*k+j+3] = (((int)hex_data[hex_i+k+HALFREAD]&0xFF)<<8) | ((int)hex_data[hex_i+k]&0xFF); //[High,Low]

                if (a_sample[6*k+j+3]> 32767) a_sample[6*k+j+3] -= 65536; // # Format correction

                a_sample[6*k+j+3] *= GSCALE; // Unit converted to g (gravity)
            }
            hex_i += READNUM;
        }
        decoded_data.push_back(a_sample); //  Append a complete sample

        dataSetNum = decoded_data.size();
        // decoded_data is a vector of size dataSetNum * 138
    }

    // output USB data from buffer
    return decoded_data;
    // Three axes: [X_L,X_H,Y_L,Y_H,Z_L,Z_H]

}

uint64_t Acciii::GetPIDTimeInNanoseconds(){
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;
    uint64_t        elapsedNano;
    static mach_timebase_info_data_t    sTimebaseInfo;

    // Start the clock.

    start = mach_absolute_time();

    // Call getpid. This will produce inaccurate results because
    // we're only making a single system call. For more accurate
    // results you should call getpid multiple times and average
    // the results.

    (void) getpid();

    // Stop the clock.

    end = mach_absolute_time();

    // Calculate the duration.

    elapsed = end - start;

    // Convert to nanoseconds.

    // If this is the first time we've run, get the timebase.
    // We can use denom == 0 to indicate that sTimebaseInfo is
    // uninitialised because it makes no sense to have a zero
    // denominator is a fraction.

    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    // Do the maths. We hope that the multiplication doesn't
    // overflow; the price you pay for working in fixed point.

    elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

    return elapsedNano;
}

int Acciii::returnDataSetNum(){
    return dataSetNum;
}

float Acciii::returnIdDataRate(){
    return idDataRate;
}

void Acciii::setSampleTime(float time){
    sampleTime = time;
    DataNum = sampleTime * ExpFs * AccBusNum * DataByteNum;
    fileBuffer = new unsigned char[DataNum];
}
