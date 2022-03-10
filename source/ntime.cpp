#include "ntime.h"

mutex ntime::timeMutex;
int ntime::timeZone = 0;
chrono::time_point<chrono::steady_clock> ntime::starttmr = chrono::steady_clock::now();
void ntime::loadNetworkTime()
{
}

time_t ntime::getTime()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	return time(&now);
}

string ntime::getDateTime()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	char strftime_buf[64];
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	return string(strftime_buf);
}

string ntime::getTimeS()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	string hr = to_string(getHour());
	string min = to_string(getMinute());
	if (hr.length() == 1) hr = "0" + hr;
	if (min.length() == 1) min = "0" + min;
	return hr + ":" + min;
}

string ntime::getTimeWithSeconds()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	string hr = to_string(getHour());
	string min = to_string(getMinute());
	string sec = to_string(getSecond());
	if (hr.length() == 1) hr = "0" + hr;
	if (min.length() == 1) min = "0" + min;
	if (sec.length() == 1) sec = "0" + sec;
	return hr + ":" + min + ":" + sec;
}

int ntime::getHour()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_hour + timeZone;
}

int ntime::getMinute()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_min;
}

int ntime::getSecond()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_sec;
}

int ntime::getYear()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_year;
}

int ntime::getMonth()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_mon;
}

int ntime::getDay()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_mday;
}

long long ntime::getMillis()
{
	lock_guard<mutex> safeLock(timeMutex);
#if defined (__ESP32)
	long long millis = esp_timer_get_time() / 1000;
#else
	long long millis = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - starttmr).count();
#endif
	return millis;
}

bool ntime::getDST()
{
	lock_guard<mutex> safeLock(timeMutex);
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_isdst;
}

void ntime::setTimeZone(int timezone)
{

}