#pragma once
#include "util.h"
#include "containers.h"

class nvm;
class iapi
{
public:
	virtual Array<byte> message(Array<byte> indata, nvm* v)
	{
		// implement for every component interface
	}
};