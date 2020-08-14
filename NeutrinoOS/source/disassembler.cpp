#include "disassembler.h"
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
	return disassembleCode(bc, size - codeindex);
}
Array<instruction>* disassembler::disassembleCode(byte* bc, int size)
{
	Array<instruction>* instr = new Array<instruction>();
	unsigned int cntr = 0;
	while (cntr < size)
	{
		byte op = bc[cntr];
		cntr += 1;
		int length;
		if (op == opcode::ST || op == opcode::INTS || op == opcode::SPUSH || op == opcode::CMPS || op == opcode::VADI)
		{
			length = bitconverter::toint32(bc, cntr);
			cntr += 4;
		}
		else if (op == opcode::NOP || op == opcode::RET || op == opcode::HALT || op == opcode::BREAK || op == opcode::SPOP)
		{
			length = 0;
		}
		else if (op == opcode::TOSTR || op == opcode::MOV || op == opcode::CONCAT || op == opcode::PARSE || op == opcode::SIZ || op == opcode::APPEND || op == opcode::ADD || op == opcode::SUB || op == opcode::MUL || op == opcode::DIV || op == opcode::INC || op == opcode::DEC || op == opcode::IMUL || op == opcode::IDIV || op == opcode::CMPI || op == opcode::CMP || op == opcode::AND || op == opcode::OR || op == opcode::XOR || op == opcode::SHL || op == opcode::SHR || op == opcode::EXTCALL || op == opcode::MOVL || op == opcode::VAD || op == opcode::VPF || op == opcode::VAR || op == opcode::VAP || op == opcode::VAL)
		{
			length = 8;
		}
		else if (op == opcode::CLR || op == opcode::CZ || op == opcode::JMP || op == opcode::JEQ || op == opcode::JNE || op == opcode::JLE || op == opcode::JGE || op == opcode::JLT || op == opcode::JGT || op == opcode::JZ || op == opcode::JNZ || op == opcode::EMIT || op == opcode::PUSH || op == opcode::POP || op == opcode::TOP || op == opcode::MOVPC || op == opcode::LJ || op == opcode::LJE || op == opcode::LJNE || op == opcode::LJG || op == opcode::LJL || op == opcode::LJGE || op == opcode::LJLE || op == opcode::NOT || op == opcode::VAC || op == opcode::VADE)
		{
			length = 4;
		}
		else if (op == opcode::SPLIT || op == opcode::INDEX || op == opcode::INSERT || op == opcode::PUSHBLK || op == opcode::VAI || op == opcode::EXTMOVL)
		{
			length = 12;
		}
		else if (op == opcode::PUSHB || op == opcode::SJ || op == opcode::POPB || op == opcode::VPUSHB || op == opcode::CLRB || op == opcode::SJE || op == opcode::SJNE || op == opcode::SJLE || op == opcode::SJGE || op == opcode::SJL || op == opcode::SJG || op == opcode::SJZ || op == opcode::SJNZ || op == opcode::CZB || op == opcode::BITS)
		{
			length = 1;
		}
		else if (op == opcode::INTR || op == opcode::STB)
		{
			length = 5;
		}
		else if (op == opcode::INTB || op == opcode::CONCATB || op == opcode::APPENDB || op == opcode::CMPB || op == opcode::CMPIB)
		{
			length = 2;
		}
		else
		{
			length = bc[cntr];
			cntr += 1;
		}
		byte* parameters = new byte[length];
		for(int i = 0; i < length; i++)
		{
			parameters[i] = bc[cntr];
			cntr++;
		}
		instr->push(instruction((opcode)op, parameters, length));
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