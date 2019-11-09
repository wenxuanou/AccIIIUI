#pragma once
// Functions for Data Processing

#include <fstream> 
#include <iostream>
#include "DataProc.h"

DWORD BytesReceived;
unsigned char RxBuffer[10000];

void USBReadData(FT_HANDLE ftHandle, DWORD readBytes, long* dwSum, int dataNum, unsigned char fileBuffer[])
{
	FT_STATUS ftStatus;
	ftStatus = FT_Read(ftHandle, RxBuffer, readBytes, &BytesReceived);
	if (ftStatus == FT_OK)
	{
		long dwSum_org = *dwSum;
		*dwSum = *dwSum + BytesReceived;

        if (*dwSum <= dataNum)
        {
            memcpy(fileBuffer + dwSum_org, RxBuffer, BytesReceived);
        }
	}
	else
	{
		// FT_Read Failed  
        //TRACE(_T("FT_Read Failed! ftStatus = %d\r\n"), ftStatus);
	}
}

void SaveDataResult(long dwSum, unsigned char fileBuffer[])
{
	FILE* fp;
	errno_t err;
//	if ((err = fopen_s(&fp, "data.bin", "wb")) != 0)
//        //TRACE("The file was not opened\n");
//	else
//        //TRACE("The file was opened\n");

    size_t write_size = fwrite(fileBuffer, sizeof(char), dwSum, fp);
    //TRACE(_T("wrote %d elements\r\n"), write_size);
	fclose(fp);
}

void SaveNum(float inputValue, std::string fileName)
{
	std::ofstream ofs;
	ofs.open(fileName);
	ofs << inputValue << std::endl;
	ofs.close();
}
