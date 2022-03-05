#include "disassembler.h"
#include "memorystats.h"
#include "kernlog.h"
Array<instruction>* disassembler::disassembleExecutable(byte* program, int size)
{
	//if (program.size() < 4) throw "Corrupted executable!";
	string magic = "";
	magic += program[0];
	magic += program[1];
	magic += program[2];
	//if (magic != "NEX") throw "Corrupted executable!";
	byte rti = program[3];
	int codeindex = 4;
	if (rti == 'L')
	{
		codeindex += 4;
		int numOfSec = bitconverter::toint32(program, 4);
		int secIterator = 0;
		while (secIterator < numOfSec)
		{
			codeindex += 8;
			secIterator += 1;
		}
	}
	byte* bc = new byte[size - codeindex];
	for (unsigned int i = codeindex; i < size; i++)
	{
		bc[i - codeindex] = program[i];
	}
	Array<instruction>* rv = disassembleCode(bc, size - codeindex);
	return rv;
}
Array<instruction>* disassembler::disassembleCode(byte* bc, int size)
{
	Array<instruction>* instr = new Array<instruction>();
	unsigned int cntr = 0;
	byte op;
	byte* parameters;
	while (cntr < size)
	{
		op = bc[cntr];
		cntr += 1;
		int length;
		if (op == opcode::LDSTR)
		{
			length = bitconverter::toint32(bc, cntr);
			cntr += 4;
		}
		else if(op == opcode::LDSTRB)
		{
			op = opcode::LDSTR;
			length = bc[cntr];
			cntr += 1;
		}
		else if (op == opcode::PUSHL || op == opcode::BR || op == opcode::JMP || op == opcode::LDI || op == opcode::LDLOC || op == opcode::STLOC || op == opcode::SWSCOP || op == opcode::EMIT || op == opcode::LDGL || op == opcode::STGL || op == opcode::TOP)
		{
			length = 4;
		}
		else if (op == opcode::CAST || op == opcode::LDGLB || op == opcode::STGLB || op == opcode::LDLOCB || op == opcode::STLOCB || op == opcode::INTR || op == opcode::LDB || op == opcode::LDIB || op == opcode::TOPB || op == opcode::SWSCOPB)
		{
			length = 1;
		}
		else if(op == opcode::PUSHLX)
		{
		    length = 8;
		}
		else if(op == opcode::LINK)
		{
		    length = bc[cntr];
		    cntr += 1;
		}
		else
		{
			length = 0;
		}
		parameters = new byte[length];
		for(int i = 0; i < length; i++)
		{
			parameters[i] = bc[cntr];
			cntr++;
		}
		instr->push(instruction((opcode)op, parameters, length));
		delete[] parameters;
	}
	delete[] bc;
	byte* np;
	for(int i = 0; i < instr->size; i++)
	{
		op = instr->get(i).opCode;
		if (op == opcode::LDGLB || op == opcode::STGLB || op == opcode::LDLOCB || op == opcode::STLOCB || op == opcode::LDIB || op == opcode::TOPB || op == opcode::SWSCOPB)
		{
			np = new byte[4];
			np[0] = instr->get(i).parameters[0];
			for(int j = 1; j < 4; j++) np[j] = 0;
			switch(op)
			{
			case opcode::LDGLB:
				instr->get(i) = instruction(opcode::LDGL, np, 4);
				break;
			case opcode::STGLB:
				instr->get(i) = instruction(opcode::STGL, np, 4);
				break;
			case opcode::LDLOCB:
				instr->get(i) = instruction(opcode::LDLOC, np, 4);
				break;
			case opcode::STLOCB:
				instr->get(i) = instruction(opcode::STLOC, np, 4);
				break;
			case opcode::LDIB:
				instr->get(i) = instruction(opcode::LDI, np, 4);
				break;
			case opcode::TOPB:
				instr->get(i) = instruction(opcode::TOP, np, 4);
				break;
			case opcode::SWSCOPB:
				instr->get(i) = instruction(opcode::SWSCOP, np, 4);
				break;
			}
			delete[] np;
		}
	}
	return instr;
}
map<int, int> disassembler::extractExternalMethods(byte* program, int size)
{
	//if (program.size() < 4) throw "Corrupted library!";
	string magic = "";
	magic += program[0];
	magic += program[1];
	magic += program[2];
	//if (magic != "NEX") throw "Corrupted library!";
	byte rti = program[3];
	int codeindex = 4;
	if (rti == 'L')
	{
		codeindex += 4;
		int numOfSec = bitconverter::toint32(program, 4);
		int secIterator = 0;
		map<int, int> secs;
		while (secIterator < numOfSec)
		{
			int sec = bitconverter::toint32(program, codeindex);
			codeindex += 4;
			int sa = bitconverter::toint32(program, codeindex);
			codeindex += 4;
			secs.insert({ sec, sa });
			secIterator += 1;
		}
		return secs;
	}
	//else throw "Invalid library!";
	return map<int, int>();
}
