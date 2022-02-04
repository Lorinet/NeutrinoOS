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
	for (int i = 0; i < v->pages.size(); i++)
	{
		v->globalPages.add(vmobject());
		v->globalPages.get(v->globalPages.size - 1).refcount = 1;
		v->localScopes.add(Array<vmobject>());
		v->currentScopes.add(Array<int>());
	}
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
		if ((*v->bytecode)[i].opCode == opcode::PUSHLX)
		{
			sec = bitconverter::toint32((*v->bytecode)[i].parameters, 0);
			key = bitconverter::tostring((*v->bytecode)[i].parameters, 4, (*v->bytecode)[i].psize);
			(*v->bytecode)[i].opCode = opcode::LDI;
			(*v->bytecode)[i].psize = 4;
			(*v->bytecode)[i].parameters = new byte[4];
			ab = bitconverter::toarray_p(offsets[key] + sections[key][sec]);
			for(int j = 0; j < 4; j++)
			{
				(*v->bytecode)[i].parameters[j] = ab[j];
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
					if ((*dasm)[j].opCode == opcode::PUSHL || (*dasm)[j].opCode == opcode::BRP)
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
		if ((*dasm)[j].opCode == opcode::PUSHLX)
		{
			modind = bitconverter::toint32((*dasm)[j].parameters, 0);
			modnm = bitconverter::toarray_p(imods[modind] + '\0');
			sec = bitconverter::toarray_p(bitconverter::toint32((*dasm)[j].parameters, 4));
			(*dasm)[j].psize = imods[bitconverter::toint32((*dasm)[j].parameters, 0)].size() + 4;
			(*dasm)[j].parameters = bitconverter::append(sec, 4, modnm, imods[modind].size());
		}
	}
	if(modnm != NULL) delete modnm;
	if(sec != NULL) delete sec;
}
