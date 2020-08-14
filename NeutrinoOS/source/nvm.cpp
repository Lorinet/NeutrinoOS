#include "nvm.h"
//#include "vt.h"
#include "timer.h"
nvm::nvm()
{
	fileName = "??";
	bytecode = new Array<instruction>();
	astack = BufferedStack(10);
	eventHandlers = map<byte, int>();
	timers = map<int, timerevt>();
	pc = 0;
	running = false;
	equal = false;
	less = false;
	greater = false;
	zero = false;
	awaitmsg = false;
	eventsenabled = true;
	processid = 0;
	extcalls = map<int, int>();
	modules = map<string, pair<int, int>>();
	lnkndx = 0;
	curPage = 0;
	bits = 32;
	awaitin = false;
	waitForProcInput = -1;
	interm = new vt(processid, -1, vtype::StandardInput);
	outterm = new vt(processid, -1, vtype::StandardOutput);
}
nvm::nvm(Array<instruction>* code)
{
	fileName = "??";
	bytecode = code;
	eventHandlers = map<byte, int>();
	astack.top = -1;
	pc = 0;
	running = false;
	equal = false;
	less = false;
	greater = false;
	zero = false;
	awaitmsg = false;
	processid = 0;
	suspended = true;
	eventsenabled = false;
	extcalls = map<int, int>();
	modules = map<string, pair<int, int>>();
	lnkndx = 0;
	curPage = 0;
	bits = 32;
	waitForProcInput = -1;
	awaitin = false;
	interm = new vt(processid, -1, vtype::StandardInput);
	outterm = new vt(processid, -1, vtype::StandardOutput);
}
void nvm::start()
{
	dynamiclinker::dynamicLink(this);
	running = true;
	suspended = false;
	eventsenabled = true;
	bits = 32;
}
void nvm::start(int procid, string file)
{
	fileName = file;
	processid = procid;
	start();
}
void nvm::cycle()
{
	if (running && !awaitmsg && !awaitin && waitForProcInput == -1)
	{
		if (pc >= bytecode->size)
		{
			running = false;
			return;
		}
		i = bytecode->get(pc);
		pc += 1;
		switch (i.opCode)
		{
		case opcode::NOP:
			break;
		case opcode::AND:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) & bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::OR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) | bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::XOR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) ^ bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SHL:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) << bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SHR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) >> bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::NOT:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			memory[curPage][bitconverter::toint32(i.parameters, 0)] = bitconverter::toArray(~k1);
			break;
		case opcode::ST:
			k = bitconverter::toint32(i.parameters, 0);
			v = Array<byte>(i.parameters, i.psize);
			v.erase(0, 4);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].emplace(k, v);
			break;
		case opcode::TOSTR:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(to_string(k1));
			break;
		case opcode::PARSE:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(stoi(bitconverter::tostring(memory[curPage][k1])));
			break;
		case opcode::CLR:
			k = bitconverter::toint32(i.parameters, 0);
			if (memory[curPage].find(k) != memory[curPage].end()) memory[curPage][k].clear();
			else
			{
				v = Array<byte>();
				memory[curPage].emplace(k, v);
			}
			break;
		case opcode::CONCAT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			bl = memory[curPage][k2];
			for (n = 0; n < memory[curPage][k1].size; n++)
			{
				bl.push(memory[curPage][k1][n]);
			}
			memory[curPage][k2] = bl;
			break;
		case opcode::MOV:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			if (memory[curPage].count(k2)) memory[curPage][k2] = memory[curPage][k1];
			else memory[curPage].emplace(k2, memory[curPage][k1]);
			break;
		case opcode::SPLIT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			k = bitconverter::toint32(i.parameters, 8);
			po = memory[curPage][k1];
			t = Array<byte>();
			pu = Array<byte>();
			for (n = 0; n < po.size; n++)
			{
				if (n < (unsigned int)k) t.push(po[n]);
				else pu.push(po[n]);
			}
			memory[curPage][k1] = t;
			memory[curPage][k2] = pu;
			break;
		case opcode::INDEX:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			if(bits == 32)
				memory[curPage][k2] = bitconverter::toArray((int)memory[curPage][bitconverter::toint32(i.parameters, 0)][k1]);
			else if(bits == 8)
			{
				po = Array<byte>();
				po.add(memory[curPage][bitconverter::toint32(i.parameters, 0)][k1]);
				memory[curPage][k2] = Array<byte>(po);
				po.clear();
			}
			break;
		case opcode::INSERT:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			memory[curPage][bitconverter::toint32(i.parameters, 0)][k1] = memory[curPage][k2][0];
			break;
		case opcode::SIZ:
			v = memory[curPage][bitconverter::toint32(i.parameters, 0)];
			k1 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k1] = bitconverter::toArray(v.size);
			break;
		case opcode::APPEND:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			bl = memory[curPage][k2];
			bl.push(memory[curPage][k1][0]);
			if (memory[curPage].count(k2)) memory[curPage][k2] = bl;
			else memory[curPage].emplace(k2, bl);
			break;
		case opcode::PUSHBLK:
			k = bitconverter::toint32(i.parameters, 0);
			k1 = bitconverter::toint32(i.parameters, 4);
			k2 = bitconverter::toint32(i.parameters, 8);
			v = Array<byte>(k2);
			for (sn = 0; sn < k2; sn++)
			{
				v[sn] = memory[curPage][k][k1 + sn];
			}
			astack.push(v);
			break;
		case opcode::STB:
			k = bitconverter::toint32(i.parameters, 0);
			if (bits == 32)
				v = bitconverter::toArray((int)i.parameters[4]);
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(i.parameters[4]);
			}
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].emplace(k, v);
			break;
		case opcode::PUSHB:
			astack.push(bitconverter::toArray((int)i.parameters[0]));
			break;
		case opcode::CONCATB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			bl = memory[curPage][k2];
			//bl.insert(&memory[curPage][k1], 0, 0, memory[curPage][k1].size);
			for (n = 0; n < memory[curPage][k1].size; n++)
			{
				bl.push(memory[curPage][k1][n]);
			}
			memory[curPage][k2] = bl;
			break;
		case opcode::APPENDB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			bl = memory[curPage][k2];
			bl.push(memory[curPage][k1][0]);
			if (memory[curPage].count(k2)) memory[curPage][k2] = bl;
			else memory[curPage].emplace(k2, bl);
			break;
		case opcode::CLRB:
			k = i.parameters[0];
			if (memory[curPage].find(k) != memory[curPage].end()) memory[curPage][k].clear();
			else
			{
				v = Array<byte>();
				memory[curPage].emplace(k, v);
			}
			break;
		case opcode::VAC:
			k1 = bitconverter::toint32(i.parameters, 0);
			ii = 0;
			while (arrays.find(ii) != arrays.end())
			{
				ii++;
			}
			arrays.emplace(ii, arrayobj());
			memory[curPage][k1] = bitconverter::toArray(ii);
			break;
		case opcode::VAD:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			arrays[k1].data.push(memory[curPage][k2]);
			break;
		case opcode::VAR:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			arrays[k1].data.removeAt(k2);
			break;
		case opcode::VAI:
			k = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0); // indexand
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0); // index
			k2 = bitconverter::toint32(i.parameters, 8); // destination
			memory[curPage][k2] = arrays[k].data[k1];
			break;
		case opcode::VADE:
			k = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			arrays.erase(k);
			break;
		case opcode::VAP:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			arrays[k2].data.insert(arrays[k1].data, arrays[k2].data.size, 0, arrays[k1].data.size);
			break;
		case opcode::VPF:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			arrays[k1].data.pushFront(memory[curPage][k2]);
			break;
		case opcode::VADI:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			v = Array<byte>(i.parameters, i.psize);
			v.erase(0, 4);
			arrays[k1].data.push(v);
			break;
		case opcode::VAL:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(arrays[k1].data.size);
			break;
		case opcode::VAS:
			k = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0); // indexand
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0); // index
			k2 = bitconverter::toint32(i.parameters, 8); // source
			arrays[k].data[k1] = memory[curPage][k2];
			break;
		case opcode::MOVL:
			k = bitconverter::toint32(i.parameters, 0);
			v = Array<byte>(i.parameters, i.psize);
			v.erase(0, 4);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].emplace(k, v);
			break;
		case opcode::ADD:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) + bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SUB:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) - bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::MUL:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) * bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::DIV:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k1], 0) / bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::INC:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k], 0) + bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::DEC:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k], 0) - bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::IMUL:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k], 0) * bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::IDIV:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toArray(bitconverter::toint32(memory[curPage][k], 0) / bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::CMP:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			cv1 = bitconverter::toint32(memory[curPage][k1], 0);
			cv2 = bitconverter::toint32(memory[curPage][k2], 0);
			less = false;
			greater = false;
			equal = false;
			zero = false;
			if (cv1 < cv2) less = true;
			if (cv1 > cv2) greater = true;
			if (cv1 == cv2) equal = true;
			break;
		case opcode::CMPB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			cv1 = bitconverter::toint32(memory[curPage][k1], 0);
			cv2 = bitconverter::toint32(memory[curPage][k2], 0);
			less = false;
			greater = false;
			equal = false;
			zero = false;
			if (cv1 < cv2) less = true;
			if (cv1 > cv2) greater = true;
			if (cv1 == cv2) equal = true;
			break;
		case opcode::CZ:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(memory[curPage][k1], 0);
			zero = false;
			if (k2 == 0) zero = true;
			break;
		case opcode::CZB:
			k1 = i.parameters[0];
			k2 = bitconverter::toint32(memory[curPage][k1], 0);
			zero = false;
			if (k2 == 0) zero = true;
			break;
		case opcode::CMPS:
			less = false;
			greater = false;
			equal = true;
			zero = false;
			v1 = memory[curPage][bitconverter::toint32(i.parameters, 0)];
			if (v1.size == i.psize - 4)
			{
				for (n = 4; n < i.psize; n++)
				{
					if (v1[n - 4] != i.parameters[n]) equal = false;
				}
			}
			else equal = false;
			break;
		case opcode::CMPI:
			less = false;
			greater = false;
			equal = false;
			zero = false;
			cmpi = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			cmpid = bitconverter::toint32(i.parameters, 4);
			if (cmpi == cmpid) equal = true;
			if (cmpi < cmpid) less = true;
			if (cmpi > cmpid) greater = true;
			break;
		case opcode::CMPIB:
			less = false;
			greater = false;
			equal = false;
			zero = false;
			cmpi = bitconverter::toint32(memory[curPage][i.parameters[0]], 0);
			cmpid = i.parameters[1];
			if (cmpi == cmpid) equal = true;
			if (cmpi < cmpid) less = true;
			if (cmpi > cmpid) greater = true;
			break;
		case opcode::JMP:
			addr = bitconverter::toint32(i.parameters, 0);
			branch(addr);
			break;
		case opcode::JEQ:
			addr = bitconverter::toint32(i.parameters, 0);
			if (equal)
			{
				branch(addr);
			}
			break;
		case opcode::JNE:
			addr = bitconverter::toint32(i.parameters, 0);
			if (!equal)
			{
				branch(addr);
			}
			break;
		case opcode::JLE:
			addr = bitconverter::toint32(i.parameters, 0);
			if (equal || less)
			{
				branch(addr);
			}
			break;
		case opcode::JGE:
			addr = bitconverter::toint32(i.parameters, 0);
			if (equal || greater)
			{
				branch(addr);
			}
			break;
		case opcode::JLT:
			addr = bitconverter::toint32(i.parameters, 0);
			if (less)
			{
				branch(addr);
			}
			break;
		case opcode::JGT:
			addr = bitconverter::toint32(i.parameters, 0);
			if (greater)
			{
				branch(addr);
			}
			break;
		case opcode::JZ:
			addr = bitconverter::toint32(i.parameters, 0);
			if (zero)
			{
				branch(addr);
			}
			break;
		case opcode::JNZ:
			addr = bitconverter::toint32(i.parameters, 0);
			if (!zero)
			{
				branch(addr);
			}
			break;
		case opcode::SJE:
			addr = i.parameters[0];
			if (equal)
			{
				branch(addr);
			}
			break;
		case opcode::SJNE:
			addr = i.parameters[0];
			if (!equal)
			{
				branch(addr);
			}
			break;
		case opcode::SJLE:
			addr = i.parameters[0];
			if (equal || less)
			{
				branch(addr);
			}
			break;
		case opcode::SJGE:
			addr = i.parameters[0];
			if (equal || greater)
			{
				branch(addr);
			}
			break;
		case opcode::SJL:
			addr = i.parameters[0];
			if (less)
			{
				branch(addr);
			}
			break;
		case opcode::SJG:
			addr = i.parameters[0];
			if (greater)
			{
				branch(addr);
			}
			break;
		case opcode::SJZ:
			addr = i.parameters[0];
			if (zero)
			{
				branch(addr);
			}
			break;
		case opcode::SJNZ:
			addr = i.parameters[0];
			if (!zero)
			{
				branch(addr);
			}
			break;
		case opcode::RET:
			if (callstack.size == 0)
			{
				halt();
			}
			else
			{
				pc = callstack.getTop();
				callstack.pop();
				for (pair<int, pair<int, int>> p : pages)
				{
					if (p.second.first <= pc && p.second.second >= pc) curPage = p.first;
				}
			}
			break;
		case opcode::EMIT:
			k = bitconverter::toint32(i.parameters, 0);
			v = memory[curPage][k];
			emc = *disassembler::disassembleExecutable(v.holder, v.size);
			addr = bytecode->size;
			for (n = 0; n < emc.size; n++) bytecode->push(emc[n]);
			branch(addr);
			break;
		case opcode::MOVPC:
			k = bitconverter::toint32(i.parameters, 0);
			v = bitconverter::toArray(pc);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].emplace(k, v);
			break;
		case opcode::LJ:
			pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJE:
			if(equal) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJNE:
			if(!equal) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJG:
			if(greater) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJL:
			if(less) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJGE:
			if(equal || greater) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJLE:
			if(equal || less) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::SJ:
			branch((int)i.parameters[0]);
			break;
		case opcode::INTS:
			v = syscall::systemCall(i.parameters, i.psize, this);
			if (v.size != 0) astack.push(v);
			break;
		case opcode::INTR:
			inter = i.parameters[0];
			k = bitconverter::toint32(i.parameters, 1);
			v = memory[curPage][k];
			v.insert(0, inter);
			bp = bitconverter::toarray_p(v);
			v = syscall::systemCall(bp, v.size, this);
			if (v.size != 0) astack.push(v);
			delete[] bp;
			break;
		case opcode::BREAK:
			// TODO: inform debugger?
			break;
		case opcode::INTB:
			inter = i.parameters[0];
			k = i.parameters[1];
			v = memory[curPage][k];
			v.insert(0, inter);
			bp = bitconverter::toarray_p(v);
			v = syscall::systemCall(bp, v.size, this);
			if (v.size != 0) astack.push(v);
			delete[] bp;
			break;
		case opcode::BITS:
			bits = i.parameters[0];
			break;
		case opcode::PUSH:
			if (bits == 32)
				astack.push(memory[curPage][bitconverter::toint32(i.parameters, 0)]);
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(memory[curPage][bitconverter::toint32(i.parameters, 0)][0]);
				astack.push(v);
			}
			break;
		case opcode::POP:
			if (astack.size != 0)
			{
				k = bitconverter::toint32(i.parameters, 0);
				if (memory[curPage].count(k)) memory[curPage][k] = astack.getTop();
				else
				{
					memory[curPage].emplace(k, Array<byte>(astack.getTop()));
				}
				if (bits == 8)
				{
					v = Array<byte>();
					v.push(0); v.push(0); v.push(0);
					memory[curPage][k].insert(v, memory[curPage][k].size, 0, v.size);
				}
				astack.pop();
			}
			break;
		case opcode::SPUSH:
			if (bits == 32)
			{
				v = Array<byte>(i.parameters, i.psize);
				astack.push(v);
			}
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(i.parameters[0]);
				astack.push(v);
			}
			break;
		case opcode::POPA:
			astack.clear();
			break;
		case opcode::SPOP:
			astack.pop();
			break;
		case opcode::POPB:
			if (astack.top != -1)
			{
				k = (int)i.parameters[0];
				if (memory[curPage].count(k)) memory[curPage][k] = astack.getTop();
				else
				{
					v = astack.getTop();
					memory[curPage].emplace(k, v);
				}
				if (bits == 8)
				{
					v = Array<byte>();
					v.push(0); v.push(0); v.push(0);
					memory[curPage][k].insert(v, memory[curPage][k].size, 0, v.size);
				}
				astack.pop();
			}
			break;
		case opcode::VPUSHB:
			if (bits == 32)
				astack.push(memory[curPage][i.parameters[0]]);
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(memory[curPage][i.parameters[0]][0]);
				astack.push(v);
			}
			break;
		case opcode::EXTCALL:
			halt("Unresolved external symbol!");
			break;
		case opcode::HALT:
			halt();
			break;
		default:
			break;
		}
	}
	else if (awaitin)
	{
		if (interm->input.size > 0)
		{
			if (interm->input[interm->input.size - 1] == '\n')
			{
				interm->input.removeAt(interm->input.size - 1);
				astack.push(interm->input);
				interm->input = Array<byte>();
				awaitin = false;
			}
		}
	}
	else if (waitForProcInput >= 0)
	{
		if (vmmgr::inputRequested(waitForProcInput))
		{
			waitForProcInput = -1;
		}
	}
}
void nvm::branch(int addr)
{
	callstack.push(pc);
	pc = addr;
	for (pair<int, pair<int, int>> p : pages)
	{
		if (p.second.first <= pc && p.second.second >= pc) curPage = p.first;
	}
}
void nvm::halt()
{
	running = false;
}
void nvm::halt(string err)
{
	running = false;
}