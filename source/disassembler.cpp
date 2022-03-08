#include "disassembler.h"
#include "memorystats.h"
#include "kernlog.h"
Array<instruction> disassembler::disassembleExecutable(byte* program, int size)
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
	return disassembleCode(bc, size - codeindex);
}
Array<instruction> disassembler::disassembleCode(byte* bc, int size)
{
	Array<instruction> instr = Array<instruction>();
	unsigned int cntr = 0;
	byte op;
	byte* parameters;
	while (cntr < size)
	{
		op = bc[cntr];
		cntr += 1;
		int length;
		if (op == opcode::OP_LDSTR)
		{
			length = bitconverter::toint32(bc, cntr);
			cntr += 4;
		}
		else if(op == opcode::OP_LDSTRB)
		{
			op = opcode::OP_LDSTR;
			length = bc[cntr];
			cntr += 1;
		}
		else if (op == opcode::OP_PUSHL || op == opcode::OP_BR || op == opcode::OP_JMP || op == opcode::OP_LDI || op == opcode::OP_LDLOC || op == opcode::OP_STLOC || op == opcode::OP_SWSCOP || op == opcode::OP_EMIT || op == opcode::OP_LDGL || op == opcode::OP_STGL || op == opcode::OP_TOP)
		{
			length = 4;
		}
		else if (op == opcode::OP_CAST || op == opcode::OP_LDGLB || op == opcode::OP_STGLB || op == opcode::OP_LDLOCB || op == opcode::OP_STLOCB || op == opcode::OP_INTR || op == opcode::OP_LDB || op == opcode::OP_LDIB || op == opcode::OP_TOPB || op == opcode::OP_SWSCOPB)
		{
			length = 1;
		}
		else if(op == opcode::OP_PUSHLX)
		{
		    length = 8;
		}
		else if(op == opcode::OP_LINK)
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
		instr.push(instruction((opcode)op, parameters, length));
		delete[] parameters;
	}
	delete[] bc;
	byte* np;
	for(int i = 0; i < instr.size; i++)
	{
		op = instr.get(i).opCode;
		if (op == opcode::OP_LDGLB || op == opcode::OP_STGLB || op == opcode::OP_LDLOCB || op == opcode::OP_STLOCB || op == opcode::OP_LDIB || op == opcode::OP_TOPB || op == opcode::OP_SWSCOPB)
		{
			np = new byte[4];
			np[0] = instr.get(i).parameters[0];
			for(int j = 1; j < 4; j++) np[j] = 0;
			switch(op)
			{
			case opcode::OP_LDGLB:
				instr.get(i) = instruction(opcode::OP_LDGL, np, 4);
				break;
			case opcode::OP_STGLB:
				instr.get(i) = instruction(opcode::OP_STGL, np, 4);
				break;
			case opcode::OP_LDLOCB:
				instr.get(i) = instruction(opcode::OP_LDLOC, np, 4);
				break;
			case opcode::OP_STLOCB:
				instr.get(i) = instruction(opcode::OP_STLOC, np, 4);
				break;
			case opcode::OP_LDIB:
				instr.get(i) = instruction(opcode::OP_LDI, np, 4);
				break;
			case opcode::OP_TOPB:
				instr.get(i) = instruction(opcode::OP_TOP, np, 4);
				break;
			case opcode::OP_SWSCOPB:
				instr.get(i) = instruction(opcode::OP_SWSCOP, np, 4);
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
