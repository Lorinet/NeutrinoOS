#include "dynamiclinker.h"
#include "disassembler.h"
#include "bitconverter.h"
#include "filesystem.h"
#include "opcode.h"
#include "nvm.h"
#include "lvmgr.h"
#include "vmmem.h"
#include "memorystats.h"
#include "kernlog.h"
void dynamiclinker::dynamicLink(nvm* v)
{
	map<string, int> offsets;
	replaceModulesByName(v->bytecode);
	v->pages.insert({ 0, pair<int, int>(0, v->bytecode->size - 1) });
	int ndx = 1;
	if (!link(v->bytecode, &offsets, &v->pages, &ndx, v->processid)) return;
	for (int i = 0; i < v->pages.size(); i++) v->globalPages->add(vmobject());
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
	int var;
	byte* ab;
	for (int i = 0; i < v->bytecode->size; i++)
	{
		if ((*v->bytecode)[i].opCode == opcode::LEAP)
		{
			sec = bitconverter::toint32((*v->bytecode)[i].parameters, 0);
			key = bitconverter::tostring((*v->bytecode)[i].parameters, 4, (*v->bytecode)[i].psize);
			ab = bitconverter::toarray_p(offsets[key] + sections[key][sec]);
			(*v->bytecode)[i].parameters = new byte[5];
			for (int j = 0; j < 4; j++) (*v->bytecode)[i].parameters[j] = ab[j];
			(*v->bytecode)[i].parameters[4] = sec;
			(*v->bytecode)[i].psize = 5;
			delete[] ab;
		}
		else if ((*v->bytecode)[i].opCode == opcode::EXTMOVL)
		{
			sec = bitconverter::toint32((*v->bytecode)[i].parameters, 0);
			var = bitconverter::toint32((*v->bytecode)[i].parameters, 4);
			key = bitconverter::tostring((*v->bytecode)[i].parameters, 8, (*v->bytecode)[i].psize);
			(*v->bytecode)[i].opCode = opcode::ST;
			(*v->bytecode)[i].psize = 8;
			(*v->bytecode)[i].parameters = new byte[8];
			ab = bitconverter::toarray_p(var);
			for(int j = 0; j < 4; j++)
			{
				(*v->bytecode)[i].parameters[j] = ab[j];
			}
			delete[] ab;
			ab = bitconverter::toarray_p(offsets[key] + sections[key][sec]);
			for(int j = 0; j < 4; j++)
			{
				(*v->bytecode)[i].parameters[j + 4] = ab[j];
			}
			delete[] ab;
		}
	}
	return;
}

bool dynamiclinker::link(Array<instruction>* v, map<string, int>* off, map<int, pair<int, int>>* pages, int* ndx, int pid)
{
	byte* bin;
	byte* parm;
	int size;
	Array<instruction>* dasm;
	string file;
	string path;
	for (int i = 0; i < v->size; i++)
	{
		if ((*v)[i].opCode == opcode::LINK)
		{
			(*v)[i].opCode = opcode::NOP;
			file = bitconverter::tostring((*v)[i].parameters, (*v)[i].psize);
			if (off->find(file) == off->end())
			{
				off->insert({ file, v->size });
				path = lvmgr::formatPath("0:\\Neutrino\\sys\\" + file);
				if (!file::fileExists(path))
				{
					vmmgr::vmmerror("Library " + file + " could not be found.", pid);
					return false;
				}
				bin = file::readAllBytes(path, &size);
				dasm = disassembler::disassembleExecutable(bin, size);
				delete[] bin;
				replaceModulesByName(dasm);
				for (int j = 0; j < dasm->size; j++)
				{
					if ((*dasm)[j].opCode == opcode::SJ || (*dasm)[j].opCode == opcode::SJE || (*dasm)[j].opCode == opcode::SJNE || (*dasm)[j].opCode == opcode::SJL || (*dasm)[j].opCode == opcode::SJG || (*dasm)[j].opCode == opcode::SJLE || (*dasm)[j].opCode == opcode::SJGE || (*dasm)[j].opCode == opcode::SJZ || (*dasm)[j].opCode == opcode::SJNZ)
					{
						switch((*dasm)[j].opCode)
						{
							case opcode::SJ:
								(*dasm)[j].opCode = opcode::JMP;
								break;
							case opcode::SJE:
								(*dasm)[j].opCode = opcode::JEQ;
								break;
							case opcode::SJNE:
								(*dasm)[j].opCode = opcode::JNE;
								break;
							case opcode::SJL:
								(*dasm)[j].opCode = opcode::JLT;
								break;
							case opcode::SJG:
								(*dasm)[j].opCode = opcode::JGT;
								break;
							case opcode::SJLE:
								(*dasm)[j].opCode = opcode::JLE;
								break;
							case opcode::SJGE:
								(*dasm)[j].opCode = opcode::JGE;
								break;
							case opcode::SJZ:
								(*dasm)[j].opCode = opcode::JZ;
								break;
							case opcode::SJNZ:
								(*dasm)[j].opCode = opcode::JNZ;
								break;
							default:
								break;
						}
						parm = bitconverter::toarray_p(((int)(*dasm)[j].parameters[0]) + v->size);
						delete[] (*dasm)[j].parameters;
						(*dasm)[j].parameters = parm;
						(*dasm)[j].psize = 4;
					}
					else if ((*dasm)[j].opCode == opcode::PUSHL)
					{
						int a = bitconverter::toint32((*dasm)[j].parameters, 0);
						a += v->size;
						parm = bitconverter::toarray_p(a);
						(*dasm)[j].parameters[0] = parm[0];
						(*dasm)[j].parameters[1] = parm[1];
						(*dasm)[j].parameters[2] = parm[2];
						(*dasm)[j].parameters[3] = parm[3];
						delete[] parm;
					}
					else if((*dasm)[j].opCode == opcode::JMP || (*dasm)[j].opCode == opcode::JEQ || (*dasm)[j].opCode == opcode::JNE || (*dasm)[j].opCode == opcode::JLT || (*dasm)[j].opCode == opcode::JGT || (*dasm)[j].opCode == opcode::JLE || (*dasm)[j].opCode == opcode::JGE || (*dasm)[j].opCode == opcode::JZ || (*dasm)[j].opCode == opcode::JNZ || (*dasm)[j].opCode == opcode::LJ || (*dasm)[j].opCode == opcode::LJE || (*dasm)[j].opCode == opcode::LJNE || (*dasm)[j].opCode == opcode::LJL || (*dasm)[j].opCode == opcode::LJG || (*dasm)[j].opCode == opcode::LJLE || (*dasm)[j].opCode == opcode::LJGE)
					{
						parm = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 0) + v->size);
						delete[] (*dasm)[j].parameters;
						(*dasm)[j].parameters = parm;
					}
				}
				pages->insert({ *ndx, pair<int, int>(v->size, v->size + dasm->size - 1) });
				(*ndx) += 1;
				v->insert(dasm, v->size, 0, dasm->size);
				dasm->clear();
				delete dasm;
				if (!link(v, off, pages, ndx, pid)) return false;
				break;
			}
		}
	}
	return true;
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
	byte* var = NULL;
	int modind = 0;
	for (int j = 0; j < dasm->size; j++)
	{
		if ((*dasm)[j].opCode == opcode::LEAP)
		{
			modnm = bitconverter::toarray_p(imods[bitconverter::toint32((*dasm)[j].parameters, 0)] + '\0');
			sec = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 4));
			(*dasm)[j].psize = imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size() + 4;
			(*dasm)[j].parameters = bitconverter::append(sec, 4, modnm, imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size());
		}
		else if ((*dasm)[j].opCode == opcode::EXTMOVL)
		{
			modind = bitconverter::toint32((*dasm)[j].parameters, 0);
			modnm = bitconverter::toarray_p(imods[modind] + '\0');
			sec = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 4));
			var = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 8));
			(*dasm)[j].psize = imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size() + 8;
			(*dasm)[j].parameters = bitconverter::append(sec, 4, var, 4);
			(*dasm)[j].parameters = bitconverter::append((*dasm)[j].parameters, 8, modnm, imods[modind].size());
		}
	}
	if(modnm != NULL) delete modnm;
	if(sec != NULL) delete sec;
}