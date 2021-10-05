#pragma once
#include "components.h"

#ifdef COMPONENT_IOMANAGER

#include "util.h"
#if defined(COMPONENT_GPIO)
#if defined(__UNIX)
#include <wiringPi.h>
#elif defined(__ESP32)
#include "driver/gpio.h"
#endif
#endif
#if defined(COMPONENT_SERIAL)
#if defined(__UNIX)
#include <wiringSerial.h>
#elif defined(__ESP32)

#endif
#endif
#include <string>
#include <vector>
#include <map>
#include "bitconverter.h"
#include "filesystem.h"
#include "iapi.h"

using namespace std;

class nvm;

enum iocmd
{
	PININIT = 0x00,
	DIGITALWRITE = 0x01,
	DIGITALREAD = 0x02,
	PWMWRITE = 0x03,
	SERINIT = 0x04,
	SERWRITE = 0x05,
	SERREAD = 0x06,
	SERREADBYTES = 0x07,
	SERREADBYTE = 0x08,
	SERAVAILABLE = 0x09,
	SERCLOSE = 0x0A
};

class iomanager_api : public iapi
{
public:
	static iomanager_api instance;
	static const int id = 2;
	static void initialize();
	Array<byte> message(Array<byte> indata, nvm* v);
};

#define PULL_NONE 0
#define PULL_DOWN 1
#define PULL_UP 2
class IOManager
{
public:
	static map<string, int> SerialIndex;
	static void Initialize();
	static string GetPortNativeName(string dev);
	static void UARTBegin(string dev, int bitrate);
	static void UARTWrite(string dev, vector<byte> data);
	static vector<byte> UARTRead(string dev);
	static vector<byte> UARTReadBytes(string dev, int bytes);
	static byte UARTReadByte(string dev);
	static int UARTGetAvailableBytes(string dev);
	static void UARTFlush(string dev);
	static void UARTClose(string dev);
	static void PinMode(int pin, bool output, int pull);
	static void PinWrite(int pin, bool val);
	static bool PinRead(int pin);
	static void PWMWrite(int pin, int val);
};
#endif