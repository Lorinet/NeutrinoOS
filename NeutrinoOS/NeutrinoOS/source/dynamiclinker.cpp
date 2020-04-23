#include "dynamiclinker.h"
#include "nvm.h"
void dynamiclinker::dynamicLink(nvm* v)
{
	map<string, int> offsets;
	replaceModulesByName(v->bytecode);
	v->pages.insert({ 0, pair<int, int>(0, v->bytecode->size - 1) });
	int ndx = 1;
	link(v->bytecode, &offsets, &v->pages, &ndx);
	map<string, map<int, int>> sections;
	int s;
	byte* modcod;
	for (pair<string, int> p : offsets)
	{
		modcod = file::readAllBytes(lvmgr::formatPath("0:\\Neutrino\\sys\\" + p.first), &s);
		sections.emplace(p.first, disassembler::extractExternalMethods(modcod, s));
	}
	int sec;
	string key;
	for (int i = 0; i < v->bytecode->size; i++)
	{
		if ((*v->bytecode)[i].opCode == opcode::EXTCALL)
		{
			sec = bitconverter::toint32((*v->bytecode)[i].parameters, 0);
			key = bitconverter::tostring((*v->bytecode)[i].parameters, 4, (*v->bytecode)[i].psize);
			(*v->bytecode)[i].opCode = opcode::JMP;
			(*v->bytecode)[i].parameters = bitconverter::toarray_p(offsets[key] + sections[key][sec]);
		}
	}
	return;
}

void dynamiclinker::link(Array<instruction>* v, map<string, int>* off, map<int, pair<int, int>>* pages, int* ndx)
{
	byte* bin;
	int size;
	Array<instruction>* dasm;
	string file;
	for (int i = 0; i < v->size; i++)
	{
		if ((*v)[i].opCode == opcode::LINK)
		{
			(*v)[i].opCode = opcode::NOP;
			file = bitconverter::tostring((*v)[i].parameters, (*v)[i].psize);
			if (off->find(file) == off->end())
			{
				off->insert({ file, v->size });
				bin = file::readAllBytes(lvmgr::formatPath("0:\\Neutrino\\sys\\" + file), &size);
				dasm = disassembler::disassembleExecutable(bin, size);
				replaceModulesByName(dasm);
				for (int j = 0; j < dasm->size; j++)
				{
					if ((*dasm)[j].opCode == opcode::SJ)
					{
						(*dasm)[j].opCode = opcode::JMP;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJE)
					{
						(*dasm)[j].opCode = opcode::JEQ;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJNE)
					{
						(*dasm)[j].opCode = opcode::JNE;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJL)
					{
						(*dasm)[j].opCode = opcode::JLT;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJG)
					{
						(*dasm)[j].opCode = opcode::JGT;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJLE)
					{
						(*dasm)[j].opCode = opcode::JLE;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJGE)
					{
						(*dasm)[j].opCode = opcode::JGE;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJZ)
					{
						(*dasm)[j].opCode = opcode::JZ;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::SJNZ)
					{
						(*dasm)[j].opCode = opcode::JNZ;
						(*dasm)[j].parameters = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
					}
					else if ((*dasm)[j].opCode == opcode::MOVL)
					{
						int a = bitconverter::toint32((*dasm)[j].parameters, 4);
						a += v->size;
						byte* vv = bitconverter::toarray_p(a);
						(*dasm)[j].parameters[4] = vv[0];
						(*dasm)[j].parameters[5] = vv[1];
						(*dasm)[j].parameters[6] = vv[2];
						(*dasm)[j].parameters[7] = vv[3];
						delete vv;
					}
				}
				pages->insert({ *ndx, pair<int, int>(v->size, v->size + dasm->size - 1) });
				(*ndx) += 1;
				v->insert(dasm, v->size, 0, dasm->size);
				link(v, off, pages, ndx);
				break;
			}
		}
	}
}

void dynamiclinker::replaceModulesByName(Array<instruction>* dasm)
{
	map<int, string> imods;
	int ili = 0;
	string linkf;
	for (int j = 0; j < dasm->size; j++)
	{
		if ((*dasm)[j].opCode == opcode::LINK)
		{
			linkf = bitconverter::tostring((*dasm)[j].parameters, (*dasm)[j].psize);
			imods.insert({ ili, linkf });
			ili += 1;
		}
	}
	byte* modnm = NULL;
	byte* sec = NULL;
	for (int j = 0; j < dasm->size; j++)
	{
		if ((*dasm)[j].opCode == opcode::EXTCALL)
		{
			modnm = bitconverter::toarray_p(imods[bitconverter::toint32((*dasm)[j].parameters, 0)]);
			sec = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 4));
			(*dasm)[j].psize = imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size() + 4;
			(*dasm)[j].parameters = bitconverter::append(sec, 4, modnm, imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size());
		}
	}
	if(modnm != NULL) delete modnm;
	if(sec != NULL) delete sec;
}