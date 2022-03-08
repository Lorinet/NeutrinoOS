#pragma once
#include "util.h"
using namespace std;
const string neutrinoOSVersion = "1.00";
const string neutrinoBuildNumber = "221";
const string neutrinoBuildDate = "Mar 2022";
const string aboutNeutrino = "Neutrino Core OS " + neutrinoOSVersion + " [build " + neutrinoBuildNumber + "] - " + neutrinoBuildDate;
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