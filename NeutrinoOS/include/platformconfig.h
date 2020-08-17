#pragma once
///                                                  /// 
/// Neutrino OS Portable Platform Buid Configuration ///
///                                                  ///

// Uncomment appropriate items to configure Neutrino Core OS for the target device

/// Base Platform
//#define __ESP32
//#define __WIN32
#define __UNIX

/// Device Class
//#define __HOLO
//#define __SANDBOX
#define __DESKTOP
//#define __THIN

/// CPU Architecture
//#define __XTENSA
//#define __ARM
#define __X86