#pragma once
#include "util.h"
using namespace std;
const string neutrinoOSVersion = "1.0f";
const string neutrinoBuildNumber = "110";
const string neutrinoBuildDate = "Apr 2020";
const string neutrinoRuntimeVersion = "1.0";
const string aboutNeutrino = "Thurs Core OS [Version " + neutrinoOSVersion + " build " + neutrinoBuildNumber + "] - " + neutrinoBuildDate;
#if defined(__WIN32)
const string neutrinoDeviceType = "WindowsPC";
#elif defined(__HOLO)
const string neutrinoDeviceType = "Visualink";
#elif defined(__WATCH)
const string neutrinoDeviceType = "SparkWatch";
#else
const string neutrinoDeviceType = "NeutrinoDevice";
#endif