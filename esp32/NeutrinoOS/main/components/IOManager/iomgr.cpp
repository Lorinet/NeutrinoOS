#include "iomgr.h"

#ifdef COMPONENT_IOMANAGER
#include "kernlog.h"
#include "nvm.h"

iomanager_api iomanager_api::instance;

void iomanager_api::initialize()
{
	instance = iomanager_api();
	IOManager::Initialize();
}

Array<byte> iomanager_api::message(Array<byte> indata, nvm* v)
{
	string txt = "";
	Array<byte> contents;
	switch (indata[0])
	{
#ifdef COMPONENT_GPIO
		case iocmd::PININIT:
			IOManager::PinMode(bitconverter::toint32(indata, 1), bitconverter::toint32(indata, 5), bitconverter::toint32(indata, 9));
			break;
		case iocmd::DIGITALREAD:
			return bitconverter::toArray((byte)IOManager::PinRead(bitconverter::toint32(indata, 1)));
		case iocmd::DIGITALWRITE:
			IOManager::PinWrite(bitconverter::toint32(indata, 1), bitconverter::toint32(indata, 5));
			break;
		case iocmd::PWMWRITE:
			IOManager::PWMWrite(bitconverter::toint32(indata, 1), bitconverter::toint32(indata, 5));
			break;
#endif
#ifdef COMPONENT_SERIAL
		case iocmd::SERINIT:
			IOManager::UARTBegin(bitconverter::tostring(indata, 5), bitconverter::toint32(indata, 1));
			break;
		case iocmd::SERAVAILABLE:
			return bitconverter::toarray(IOManager::UARTGetAvailableBytes(bitconverter::tostring(indata, 1)));
		case iocmd::SERCLOSE:
			IOManager::UARTClose(bitconverter::tostring(indata, 1));
			break;
		case iocmd::SERREADBYTE:
			return vector<byte>({ IOManager::UARTReadByte(bitconverter::tostring(indata, 1)), 0, 0, 0 });
		case iocmd::SERREADBYTES:
			return IOManager::UARTReadBytes(bitconverter::tostring(indata, 5), bitconverter::toint32(indata, 1));
		case iocmd::SERREAD:
			return IOManager::UARTRead(bitconverter::tostring(indata, 1));
		case iocmd::SERWRITE:
			txt = bitconverter::readto0(indata, 1);
			contents = bitconverter::subvector(indata, 2 + txt.size());
			IOManager::UARTWrite(txt, contents);
			break;
#endif
	}
	return Array<byte>();
}

map<string, int> IOManager::SerialIndex;
string IOManager::GetPortNativeName(string dev)
{
#if defined(__UNIX)
	string port = "/dev/null";
	if (util::startsWith(dev, "ser") && dev.size() == 4)
	{
		port = "/dev/ttyAMA" + (dev[3] - '0');
	}
	else if (util::startsWith(dev, "is") && dev.size() == 3)
	{
		port = "/dev/ttyS" + (dev[2] - '0');
	}
	if (!file::fileExists(port))
		port = "/dev/null";
	return port;
#elif defined(__WIN32)
	string port = "COM" + dev[dev.size() - 1];
	return port;
#endif
	return "";
}
void IOManager::Initialize()
{
	#if defined(COMPONENT_GPIO)
#if defined(__UNIX)
	wiringPiSetup();
	#elif defined(__ESP32)

	#endif
#endif
	klog("IOManager", "Started");
}
void IOManager::UARTBegin(string dev, int bitrate)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	SerialIndex.insert({dev, serialOpen(GetPortNativeName(dev).c_str(), bitrate)});
#endif
}
int IOManager::UARTGetAvailableBytes(string dev)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	return serialDataAvail(SerialIndex[dev]);
#endif
	return 0;
}
void IOManager::UARTFlush(string dev)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	serialFlush(SerialIndex[dev]);
#endif
}
byte IOManager::UARTReadByte(string dev)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	return serialGetchar(SerialIndex[dev]);
#endif
	return 0;
}
vector<byte> IOManager::UARTRead(string dev)
{
#if defined(__UNIX) && defined(COMPONENT_GPIO)
	vector<byte> v;
	int l = UARTGetAvailableBytes(dev);
	for (int i = 0; i < l; i++)
	{
		v.push_back(UARTReadByte(dev));
	}
	return v;
#endif
	return vector<byte>();
}
vector<byte> IOManager::UARTReadBytes(string dev, int bytes)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	vector<byte> v;
	int l = UARTGetAvailableBytes(dev);
	if (l > bytes)
		l = bytes;
	for (int i = 0; i < l; i++)
	{
		v.push_back(UARTReadByte(dev));
	}
	return v;
#endif
	return vector<byte>();
}
void IOManager::UARTWrite(string dev, vector<byte> indata)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	serialPuts(SerialIndex[dev], bitconverter::tostring(indata).c_str());
#endif
}
void IOManager::UARTClose(string dev)
{
#if defined(__UNIX) && defined(COMPONENT_SERIAL)
	serialClose(SerialIndex[dev]);
#endif
}
void IOManager::PinMode(int pin, bool output, int pull)
{
#if defined(COMPONENT_GPIO)
#if defined(__UNIX)
	pinMode(pin, output);
	if (!output)
		pullUpDnControl(pin, pull);
#elif defined(__ESP32)
	gpio_mode_t gmt = GPIO_MODE_INPUT;
	if(output) gmt = GPIO_MODE_INPUT_OUTPUT;
	gpio_set_direction((gpio_num_t)pin, gmt);
	if (pull == PULL_DOWN)
	{
		gpio_pulldown_en((gpio_num_t)pin);
		gpio_pullup_dis((gpio_num_t)pin);
	}
	else if (pull == PULL_UP)
	{
		gpio_pullup_en((gpio_num_t)pin);
		gpio_pulldown_dis((gpio_num_t)pin);
	}
	else if (pull == PULL_NONE)
	{
		gpio_pullup_dis((gpio_num_t)pin);
		gpio_pulldown_dis((gpio_num_t)pin);
	}
#endif
#endif
}
void IOManager::PinWrite(int pin, bool value)
{
#if defined(COMPONENT_GPIO)
#if defined(__UNIX)
	digitalWrite(pin, value);
#elif defined(__ESP32)
	gpio_set_level((gpio_num_t)pin, (uint32_t)value);
#endif
#endif
}
void IOManager::PWMWrite(int pin, int value)
{
#if defined(__UNIX) && defined(COMPONENT_GPIO)
	analogWrite(pin, value);
#endif
}
bool IOManager::PinRead(int pin)
{
#if defined(COMPONENT_GPIO)
#if defined(__UNIX)
	return digitalRead(pin);
#elif defined(__ESP32)
	return gpio_get_level((gpio_num_t)pin);
#endif
#endif
	return false;
}
#endif