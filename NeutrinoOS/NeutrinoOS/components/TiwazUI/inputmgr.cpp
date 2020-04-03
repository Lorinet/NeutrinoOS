#include "inputmgr.h"
int inputmgr::prevStatBack = 0;
int inputmgr::prevStatEnter = 0;
int inputmgr::prevStatSelect = 0;
void inputmgr::initialize()
{
	wiringPiSetup();
	pinMode(PIN_BACK, INPUT);
	pinMode(PIN_ENTER, INPUT);
	pinMode(PIN_SELECT, INPUT);
}
void inputmgr::writePinState(int pin, int state)
{
	digitalWrite(pin, state);
}
int inputmgr::readPinState(int pin)
{
	return digitalRead(pin);
}
void inputmgr::poll()
{
	if (readPinState(PIN_BACK) == 1 && prevStatBack == 0)
	{
		ViewManager::FireEvent(TiwazEvent::BackButton);
		prevStatBack = 1;
	}
	if (readPinState(PIN_ENTER) == 1 && prevStatEnter == 0)
	{
		ViewManager::FireEvent(TiwazEvent::EnterButton);
		prevStatEnter = 1;
	}
	if (readPinState(PIN_SELECT) == 1 && prevStatSelect == 0)
	{
		ViewManager::FireEvent(TiwazEvent::SelectButton);
		prevStatSelect = 1;
	}
	prevStatBack = readPinState(PIN_BACK);
	prevStatEnter = readPinState(PIN_ENTER);
	prevStatSelect = readPinState(PIN_SELECT);
}