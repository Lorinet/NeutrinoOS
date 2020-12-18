#include "inputmgr.h"
int inputmgr::prevStatBack = 0;
int inputmgr::prevStatEnter = 0;
int inputmgr::prevStatSelect = 0;
void inputmgr::initialize()
{
	IOManager::PinMode(PIN_BACK, false, PULL_UP);
	IOManager::PinMode(PIN_ENTER, false, PULL_UP);
	IOManager::PinMode(PIN_SELECT, false, PULL_UP);
}
void inputmgr::poll()
{
	if (IOManager::PinRead(PIN_BACK) == 0 && prevStatBack == 1)
	{
		ViewManager::FireEvent(TiwazEvent::BackButton);
		prevStatBack = 1;
	}
	if (IOManager::PinRead(PIN_ENTER) == 0 && prevStatEnter == 1)
	{
		ViewManager::FireEvent(TiwazEvent::EnterButton);
		prevStatEnter = 1;
	}
	if (IOManager::PinRead(PIN_SELECT) == 0 && prevStatSelect == 1)
	{
		ViewManager::FireEvent(TiwazEvent::SelectButton);
		prevStatSelect = 1;
	}
	prevStatBack = IOManager::PinRead(PIN_BACK);
	prevStatEnter = IOManager::PinRead(PIN_ENTER);
	prevStatSelect = IOManager::PinRead(PIN_SELECT);
}