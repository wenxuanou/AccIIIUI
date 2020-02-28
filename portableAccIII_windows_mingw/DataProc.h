#pragma once
//------------------------------------------------------------------------------
#include<string>
#include <fstream>
#include <iostream>
#include"ftd2xx.h"
#define ExpFs 1310 // Expected sampling frequency
#define AccBusNum 23 // Totally 23 buses each connecting two accelerometers
#define DataByteNum 12 //Each bus connects two accelerometer each with 3 axies [High + Low] bytes

#pragma comment (lib, "ftd2xx.lib")

#include <stdint.h>

void USBReadData(FT_HANDLE ftHandle, DWORD readBytes, long* dwSum, int dataNum, unsigned char fileBuffer[]);

void SaveDataResult(long dwSum, unsigned char fileBuffer[], std::string fileName);

void SaveNum(float inputValue, std::string fileName);
