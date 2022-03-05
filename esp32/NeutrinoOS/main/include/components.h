#pragma once
// Kernel component configuration
// Modify this to enable/disable drivers and other components

#define COMPONENT_IOMANAGER
#define COMPONENT_GPIO
//#define COMPONENT_SERIAL
#define COMPONENT_TIWAZ
//#define COMPONENT_EFFIGY

// Include files for components

#ifdef COMPONENT_TIWAZ
#include "TiwazUI/viewmgr.h"
#include "TiwazUI/bitmaps.h"
#include "TiwazUI/view.h"
#include "TiwazUI/elements.h"
#include "TiwazUI/graphics.h"
#include "TiwazUI/uiserialization.h"
#include "TiwazUI/inputmgr.h"
#elif defined(COMPONENT_EFFIGY)
#include "EffigyWM/element.h"
#include "EffigyWM/graphics.h"
#include "EffigyWM/serialization.h"
#include "EffigyWM/window.h"
#include "EffigyWM/winmgr.h"
#elif defined(COMPONENT_IOMANAGER)
#include "IOManager/iomgr.h"
#endif

class components
{
public:
	static void registerComponents();
};
