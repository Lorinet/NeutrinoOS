#pragma once
#include "util.h"
#define __HOLO
using namespace std;
const string neutrinoOSVersion = "1.21";
const string neutrinoBuildNumber = "110";
const string neutrinoBuildDate = "Oct 2019";
const string neutrinoRuntimeVersion = "1.2";
const string aboutNeutrino = "Thorbjorn Core OS Version " + neutrinoOSVersion + " (build " + neutrinoBuildNumber + ") - " + neutrinoBuildDate;
#if defined(__WIN32)
const string neutrinoDeviceType = "WindowsPC";
#elif defined(__UNIX)
const string neutrinoDeviceType = "NeutrinoDevice";
#else
const string neutrinoDeviceType = "NeutrinoRT";
#endif