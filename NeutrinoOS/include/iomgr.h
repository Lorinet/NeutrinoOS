#pragma once
#include "util.h"
//#define FEATURE_SERIAL
#define FEATURE_GPIO
#if defined(FEATURE_GPIO)
#if defined(__UNIX)
#include <wiringPi.h>
#elif defined(__ESP32)
#include "driver/gpio.h"
#endif
#endif
#if defined(FEATURE_SERIAL)
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
using namespace std;
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