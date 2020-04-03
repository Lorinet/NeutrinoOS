#include "ntime.h"
int ntime::timeZone = 0;
chrono::time_point<chrono::steady_clock> ntime::starttmr = chrono::steady_clock::now();
void ntime::loadNetworkTime()
{
}

time_t ntime::getTime()
{
	time_t now;
	return time(&now);
}

string ntime::getDateTime()
{
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
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_hour + timeZone;
}

int ntime::getMinute()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_min;
}

int ntime::getSecond()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_sec;
}

int ntime::getYear()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_year;
}

int ntime::getMonth()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_mon;
}

int ntime::getDay()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_mday;
}

long long ntime::getMillis()
{
#if defined (__ESP32)
	long long millis = esp_timer_get_time() / 1000;
#else
	long long millis = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - starttmr).count();
#endif
	return millis;
}

bool ntime::getDST()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	timeinfo = *localtime(&now);
	return timeinfo.tm_isdst;
}

void ntime::setTimeZone(int timezone)
{

}