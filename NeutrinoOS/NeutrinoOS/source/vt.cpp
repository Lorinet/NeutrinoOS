#include "vt.h"
vt::vt()
{
	input = Array<byte>();
}
vt::vt(int proc, int targ, vtype vty)
{
	process = proc;
	target = targ;
	type = vty;
	input = Array<byte>();
}
void vt::write(Array<byte> data)
{
	if (type == vtype::MessageOut)
	{
		vmmgr::sendMessage(target, data);
	}
	else if (type == vtype::StandardOutput)
	{
		// implement console
		cout << bitconverter::tostring(data);
	}
}
void vt::write(string data)
{
	write(bitconverter::toArray(data));
}
void vt::waitForInput()
{
	// input logic ahead!
	while (input[input.size - 1] != '\n')
	{
		vmmgr::stepAll();
	}
	input.removeAt(input.size - 1);
	input = Array<byte>();
}
void vt::pressKey(byte b)
{
	input.push(b);
}
void vt::registerInputData(Array<byte> data)
{
	input.insert(data, input.size, 0, data.size);
}
void vt::registerInputString(string data)
{
	registerInputData(bitconverter::toArray(data));
}