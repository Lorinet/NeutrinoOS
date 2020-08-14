#include "iomgr.h"
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
	#if defined(FEATURE_GPIO)
#if defined(__UNIX)
	wiringPiSetup();
	#elif defined(__ESP32)

	#endif
#endif
}
void IOManager::UARTBegin(string dev, int bitrate)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	SerialIndex.insert({dev, serialOpen(GetPortNativeName(dev).c_str(), bitrate)});
#endif
}
int IOManager::UARTGetAvailableBytes(string dev)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	return serialDataAvail(SerialIndex[dev]);
#endif
	return 0;
}
void IOManager::UARTFlush(string dev)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	serialFlush(SerialIndex[dev]);
#endif
}
byte IOManager::UARTReadByte(string dev)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	return serialGetchar(SerialIndex[dev]);
#endif
	return 0;
}
vector<byte> IOManager::UARTRead(string dev)
{
#if defined(__UNIX) && defined(FEATURE_GPIO)
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
#if defined(__UNIX) && defined(FEATURE_SERIAL)
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
void IOManager::UARTWrite(string dev, vector<byte> data)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	serialPuts(SerialIndex[dev], bitconverter::tostring(data).c_str());
#endif
}
void IOManager::UARTClose(string dev)
{
#if defined(__UNIX) && defined(FEATURE_SERIAL)
	serialClose(SerialIndex[dev]);
#endif
}
void IOManager::PinMode(int pin, bool output, int pull)
{
#if defined(FEATURE_GPIO)
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
#if defined(FEATURE_GPIO)
#if defined(__UNIX)
	digitalWrite(pin, value);
#elif defined(__ESP32)
	gpio_set_level((gpio_num_t)pin, (uint32_t)value);
#endif
#endif
}
void IOManager::PWMWrite(int pin, int value)
{
#if defined(__UNIX) && defined(FEATURE_GPIO)
	analogWrite(pin, value);
#endif
}
bool IOManager::PinRead(int pin)
{
#if defined(FEATURE_GPIO)
#if defined(__UNIX)
	return digitalRead(pin);
#elif defined(__ESP32)
	return gpio_get_level((gpio_num_t)pin);
#endif
#endif
	return false;
}