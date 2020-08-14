#pragma once
#include "util.h"
using namespace std;
const string neutrinoOSVersion = "1.0f";
const string neutrinoBuildNumber = "111";
const string neutrinoBuildDate = "Jun 2020";
const string neutrinoRuntimeVersion = "1.0";
const string aboutNeutrino = "Thurs Core OS [Version " + neutrinoOSVersion + " build " + neutrinoBuildNumber + "] - " + neutrinoBuildDate;
#if defined(__DESKTOP)
const string neutrinoDeviceType = "PC";
#elif defined(__HOLO)
const string neutrinoDeviceType = "Visualink";
#elif defined(__WATCH)
const string neutrinoDeviceType = "SparkWatch";
#else
const string neutrinoDeviceType = "NeutrinoDevice";
#endif
#if defined(__X86)
const string platformArch = "x86";
#elif defined(__XTENSA)
const string platformArch = "xtensa";
#elif defined(__ARM)
const string platformArch = "arm";
#else
const string platformArch = "unknown";
#endif