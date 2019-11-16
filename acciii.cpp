#include "acciii.h"

AccIII::AccIII(int argc, char** argv)
{
    dwSum = 0;
    fileBuffer = NULL;
    DataNum = 40000 * 24;

    setupUSB(argc, argv);
}

AccIII::~AccIII(){}

void AccIII::setupUSB(int argc, char** argv){
    std::cout<<"setup USB"<<std::endl;
    if (argc == 2)
    {
        float samp_time = std::atof(argv[1]);
        printf("Sample time = %.4f secs\r\n", samp_time);
        DataNum = (int)(samp_time * ExpFs * AccBusNum * DataByteNum);
    }
    else if (argc == 1)
    {
        printf("Default sample time = 1 secs\r\n");
        DataNum = ExpFs * AccBusNum * DataByteNum;
    }
    else
    {
        printf("Only one input argument is allowed!\r\n");
        printf("Default sample time = 1.159 secs\r\n");
    }

    fileBuffer = new unsigned char[DataNum];

    Mask = 0xff;
    LatencyTimer = 2;
    TxBuffer[1] = { 0x55 };

    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus != FT_OK)
    {
        //TRACE(_T("FT_Open FAILED!\r\n"));
        printf("FT_Open FAILED!\r\n");
    }
    else
    {
        printf("FT_Open Succeeded!\r\n");
    }

    Mode = 0x00; //reset mode
    ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);

}

void AccIII::transmitData(){
    if (ftStatus == FT_OK)
    {
        ftStatus = FT_SetLatencyTimer(ftHandle, LatencyTimer);
        if (ftStatus == FT_OK) {
            //TRACE(_T("FT_SetLatencyTimer OK!\r\n"));
            printf("FT_SetLatencyTimer Succeeded!\r\n");
        }
        else {
            //TRACE(_T("FT_SetLatencyTimer FAILED!\r\n"));
            printf("FT_SetLatencyTimer FAILED!\r\n");
        }

        ftStatus = FT_SetUSBParameters(ftHandle, 0x10000, 0x1);
        if (ftStatus == FT_OK) {
            //TRACE(_T("FT_SetUSBParameters OK!\r\n"));
            printf("FT_SetUSBParameters Succeeded!\r\n");
        }
        else {
            //TRACE(_T("FT_SetUSBParameters FAILED!\r\n"));
            printf("FT_SetUSBParameters FAILED!\r\n");
        }

        ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0);
        if (ftStatus == FT_OK) {
            //TRACE(_T("FT_SetFlowControl OK!\r\n"));
            printf("FT_SetFlowControl Succeeded!\r\n");
        }
        else {
            //TRACE(_T("FT_SetFlowControl FAILED!\r\n"));
            printf("FT_SetFlowControl FAILED!\r\n");
        }

        ftStatus = FT_Purge(ftHandle, FT_PURGE_RX);
        if (ftStatus == FT_OK) {
            //TRACE(_T("FT_Purge OK!\r\n"));
            printf("FT_Purge Succeeded!\r\n");
        }
        else {
            //TRACE(_T("FT_Purge FAILED!\r\n"));
            printf("FT_Purge FAILED!\r\n");
        }

        ftStatus = FT_Write(ftHandle, TxBuffer, sizeof(TxBuffer), &BytesWritten);
        if (ftStatus == FT_OK) {
            //TRACE(_T("FT_Write OK!\r\n"));
            printf("FT_Write Succeeded!\r\n");
        }
        else {
            //TRACE(_T("FT_Write Failed\r\n"));
            printf("FT_Write Failed\r\n");
        }

        dwSum = 0;
        readBytesTest = 0;

        std::cout<<"begin sampling"<<std::endl;
        printf("Sampling...\n");


        // Extracting data in while loop
        while (dwSum < DataNum)
        {
            usleep(500000);

            std::cout << "Thread using function"
                   " pointer as callable\n";

            // Read in data from RxButter
           // ftStatus = FT_Read(ftHandle, RxBuffer, readBytesTest, &BytesReceivedTest);

            if ((ftStatus == FT_OK) && (RxBytes > 0))
            {
                if (RxBytes < 10000)
                {
                    USBReadData(ftHandle, RxBytes, &dwSum, DataNum, fileBuffer);
                }
                else
                {
                    for (int i = 0; i <  RxBytes / 10000; i++)
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


    }
    else {
        // FT_SetBitMode FAILED!
        FT_Close(ftHandle);
    }

    free(fileBuffer); // Free buffer memory to avoid memory leak
}

void stopTransmission(){

}


std::vector<std::vector<float>> AccIII::printData(){
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
        std::vector<float> a_sample(3*READNUM); // [Acc0_X,Acc0_Y,Acc0_Z,Acc1_X,...,Acc46_Z] 46*3 = 138
		
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
}
// Three axes: [X_L,X_H,Y_L,Y_H,Z_L,Z_H]

int AccIII::printDataSetNum(){
    return dataSetNum;
}

