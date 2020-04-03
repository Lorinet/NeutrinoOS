#include "vt.h"
vt::vt()
{
	buffer = deque<vector<byte>>();
	input = vector<byte>();
}
vt::vt(int proc, int targ, vtype vty)
{
	process = proc;
	target = targ;
	type = vty;
	buffer = deque<vector<byte>>();
	input = vector<byte>();
}
void vt::write(vector<byte> data)
{
	buffer.push_back(data);
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
	write(bitconverter::toarray(data));
}
vector<byte> vt::readAllData()
{
	vector<byte> vb;
	for (int i = 0; i < buffer.size(); i++)
	{
		vb.insert(vb.end(), buffer[i].begin(), buffer[i].end());
		vb.push_back('\n');
	}
	return vb;
}
vector<byte> vt::readLine()
{
	return buffer[buffer.size() - 1];
}
string vt::readLineStr()
{
	return bitconverter::tostring(readLine());
}
string vt::readAllStr()
{
	return bitconverter::tostring(readAllData());
}
void vt::waitForInput()
{
	// input logic ahead!
	while (input[input.size() - 1] != '\n')
	{
		vmmgr::stepAll();
	}
	input.erase(input.end() - 1);
	buffer.push_back(input);
	input = vector<byte>();
}
void vt::pressKey(byte b)
{
	input.push_back(b);
}
void vt::registerInputData(vector<byte> data)
{
	input.insert(input.end(), data.begin(), data.end());
}
void vt::registerInputString(string data)
{
	registerInputData(bitconverter::toarray(data));
}