#pragma once
#define FEATURE_SERIAL
#define FEATURE_GPIO
#ifdef FEATURE_GPIO
#include <wiringPi.h>
#endif
#ifdef FEATURE_SERIAL
#include <wiringSerial.h>
#endif
#include <string>
#include <vector>
#include <map>
#include "util.h"
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