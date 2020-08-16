#pragma once
#if defined(__HOLO)
#define COMPONENT_TIWAZ
#elif defined(__DESKTOP)
#define COMPONENT_EFFIGY
#endif
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
#endif