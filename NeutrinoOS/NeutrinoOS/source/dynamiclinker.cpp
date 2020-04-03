#include "dynamiclinker.h"
#include "nvm.h"
void dynamiclinker::dynamicLink(nvm* v)
{
	map<string, int> offsets;
	replaceModulesByName(&v->bytecode);
	v->pages.insert({ 0, pair<int, int>(0, v->bytecode.size() - 1) });
	int ndx = 1;
	link(&v->bytecode, &offsets, &v->pages, &ndx);
	map<string, map<int, int>> sections;
	for (pair<string, int> p : offsets)
	{
		sections.insert({ p.first, disassembler::extractExternalMethods(file::readAllBytes(lvmgr::formatPath("0:\\Neutrino\\sys\\" + p.first))) });
	}
	for (int i = 0; i < v->bytecode.size(); i++)
	{
		if (v->bytecode[i].opCode == opcode::EXTCALL)
		{
			int sec = bitconverter::toint32(v->bytecode[i].parameters, 0);
			string key = bitconverter::tostring(vector<byte>(v->bytecode[i].parameters.begin() + 4, v->bytecode[i].parameters.end()));
			v->bytecode[i].opCode = opcode::JMP;
			v->bytecode[i].parameters = bitconverter::toarray(offsets[key] + sections[key][sec]);
		}
	}
	return;
}

void dynamiclinker::link(vector<instruction>* v, map<string, int>* off, map<int, pair<int, int>>* pages, int* ndx)
{
	for (int i = 0; i < v->size(); i++)
	{
		if ((*v)[i].opCode == opcode::LINK)
		{
			(*v)[i].opCode = opcode::NOP;
			string file = bitconverter::tostring((*v)[i].parameters);
			if (off->find(file) == off->end())
			{
				off->insert({ file, v->size() });
				vector<byte> bin = file::readAllBytes(lvmgr::formatPath("0:\\Neutrino\\sys\\" + file));
				vector<instruction> dasm = disassembler::disassembleExecutable(bin);
				replaceModulesByName(&dasm);
				for (int j = 0; j < dasm.size(); j++)
				{
					if (dasm[j].opCode == opcode::SJ)
					{
						dasm[j].opCode = opcode::JMP;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJE)
					{
						dasm[j].opCode = opcode::JEQ;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJNE)
					{
						dasm[j].opCode = opcode::JNE;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJL)
					{
						dasm[j].opCode = opcode::JLT;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJG)
					{
						dasm[j].opCode = opcode::JGT;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJLE)
					{
						dasm[j].opCode = opcode::JLE;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJGE)
					{
						dasm[j].opCode = opcode::JGE;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJZ)
					{
						dasm[j].opCode = opcode::JZ;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::SJNZ)
					{
						dasm[j].opCode = opcode::JNZ;
						dasm[j].parameters = bitconverter::toarray(((int)dasm[j].parameters[0]) + v->size());
					}
					else if (dasm[j].opCode == opcode::MOVL)
					{
						int a = bitconverter::toint32(dasm[j].parameters, 4);
						a += v->size();
						vector<byte> v = bitconverter::toarray(a);
						dasm[j].parameters[4] = v[0];
						dasm[j].parameters[5] = v[1];
						dasm[j].parameters[6] = v[2];
						dasm[j].parameters[7] = v[3];
					}
				}
				pages->insert({ *ndx, pair<int, int>(v->size(), v->size() + dasm.size() - 1) });
				(*ndx) += 1;
				v->insert(v->end(), dasm.begin(), dasm.end());
				link(v, off, pages, ndx);
				break;
			}
		}
	}
}

void dynamiclinker::replaceModulesByName(vector<instruction>* dasm)
{
	map<int, string> imods;
	int ili = 0;
	for (int j = 0; j < dasm->size(); j++)
	{
		if ((*dasm)[j].opCode == opcode::LINK)
		{
			string linkf = bitconverter::tostring((*dasm)[j].parameters);
			imods.insert({ ili, linkf });
			ili += 1;
		}
	}
	for (int j = 0; j < dasm->size(); j++)
	{
		if ((*dasm)[j].opCode == opcode::EXTCALL)
		{
			vector<byte> modnm = bitconverter::toarray(imods[bitconverter::toint32((*dasm)[j].parameters, 0)]);
			vector<byte> sec = bitconverter::toarray(bitconverter::toint32((*dasm)[j].parameters, 4));
			(*dasm)[j].parameters.clear();
			(*dasm)[j].parameters.insert((*dasm)[j].parameters.end(), sec.begin(), sec.end());
			(*dasm)[j].parameters.insert((*dasm)[j].parameters.end(), modnm.begin(), modnm.end());
		}
	}
}