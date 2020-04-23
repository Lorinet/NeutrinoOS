#pragma once
#if defined(__HOLO)
#define COMPONENT_TIWAZ
#endif
#ifdef COMPONENT_TIWAZ
#include "TiwazUI/viewmgr.h"
#include "TiwazUI/bitmaps.h"
#include "TiwazUI/view.h"
#include "TiwazUI/elements.h"
#include "TiwazUI/graphics.h"
#include "TiwazUI/uiserialization.h"
#include "TiwazUI/inputmgr.h"
enum uicmd
{
	CreateView = 0,
	DestroyView = 1,
	AddElement = 2,
	ModifyElement = 3,
	DeleteElement = 4,
	GetPropertyValue = 5,
	SetPropertyValue = 6,
	SwitchView = 7,
	UpdateScreen = 8,
	AttachEventHandler = 9,
	DetachEventHandler = 10
};
#endif