#include "nvm.h"
//#include "vt.h"
#include "timer.h"
nvm::nvm()
{
	bytecode = vector<instruction>();
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
nvm::nvm(vector<instruction> code)
{
	bytecode = code;
	eventHandlers = map<byte, int>();
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
void nvm::start(int procid)
{
	processid = procid;
	start();
}
void nvm::cycle()
{
	if (running && !awaitmsg && !awaitin && waitForProcInput == -1)
	{
		if (pc >= bytecode.size())
		{
			running = false;
			return;
		}
		instruction i = bytecode[pc];
		pc += 1;
		unsigned int n;
		int sn;
		int k, k1, k2, cv1, cv2, addr, cmpi, cmpid, ii;
		vector<byte> v, v1, bl, t, pu, po, arr1, arr2;
		map<int, int> sectionMap;
		string rep, lnkf;
		vector<instruction> emc;
		byte inter;
		map<string, int> adlinks;
		char c;
		switch (i.opCode)
		{
		case opcode::NOP:
			break;
		case opcode::AND:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) & bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::OR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) | bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::XOR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) ^ bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SHL:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) << bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SHR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) >> bitconverter::toint32(memory[curPage][k2], 0));
			break;

		case opcode::NOT:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			memory[curPage][bitconverter::toint32(i.parameters, 0)] = bitconverter::toarray(~k1);
			break;
		case opcode::ST:
			k = bitconverter::toint32(i.parameters, 0);
			v = i.parameters;
			v.erase(v.begin(), v.begin() + 4);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].insert({ k, v });
			break;
		case opcode::TOSTR:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(to_string(k1));
			break;
		case opcode::PARSE:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(stoi(bitconverter::tostring(memory[curPage][k1])));
			break;
		case opcode::CLR:
			k = bitconverter::toint32(i.parameters, 0);
			if (memory[curPage].count(k))
				memory[curPage].erase(k);
			else memory[curPage].insert({ k, vector<byte>() });
			break;
		case opcode::CONCAT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			bl = memory[curPage][k2];
			bl.insert(bl.end(), memory[curPage][k1].begin(), memory[curPage][k1].end());
			memory[curPage][k2] = bl;
			break;
		case opcode::MOV:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = memory[curPage][k1];
			if (memory[curPage].count(k2)) memory[curPage][k2] = memory[curPage][k1];
			else memory[curPage].insert({ k2, memory[curPage][k1] });
			break;
		case opcode::SPLIT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			k = bitconverter::toint32(i.parameters, 8);
			po = memory[curPage][k1];
			t = vector<byte>();
			pu = vector<byte>();
			for (n = 0; n < po.size(); n++)
			{
				if (n < (unsigned int)k) t.push_back(po[n]);
				else pu.push_back(po[n]);
			}
			memory[curPage][k1] = t;
			memory[curPage][k2] = pu;
			break;
		case opcode::INDEX:
			v = memory[curPage][bitconverter::toint32(i.parameters, 0)];
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			memory[curPage][k2] = bitconverter::toarray((int)v[k1]);
			break;
		case opcode::INSERT:
			v = memory[curPage][bitconverter::toint32(i.parameters, 0)];
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			memory[curPage][bitconverter::toint32(i.parameters, 0)][k1] = memory[curPage][k2][0];
			break;
		case opcode::SIZ:
			v = memory[curPage][bitconverter::toint32(i.parameters, 0)];
			k1 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k1] = bitconverter::toarray(v.size());
			break;
		case opcode::APPEND:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			bl = memory[curPage][k2];
			bl.push_back(memory[curPage][k1][0]);
			if (memory[curPage].count(k2)) memory[curPage][k2] = bl;
			else memory[curPage].insert({ k2, bl });
			break;
		case opcode::PUSHBLK:
			k = bitconverter::toint32(i.parameters, 0);
			k1 = bitconverter::toint32(i.parameters, 4);
			k2 = bitconverter::toint32(i.parameters, 8);
			v = vector<byte>(k2);
			for (sn = 0; sn < k2; sn++)
			{
				v[sn] = memory[curPage][k][k1 + sn];
			}
			astack.push(v);
			break;
		case opcode::STB:
			k = bitconverter::toint32(i.parameters, 0);
			if (bits == 32)
				v = bitconverter::toarray((int)i.parameters[4]);
			else if (bits == 8)
				v = { i.parameters[4] };
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].insert({ k, v });
			break;
		case opcode::PUSHB:
			astack.push(bitconverter::toarray((int)i.parameters[0]));
			break;
		case opcode::CONCATB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			bl = vector<byte>();
			for (byte bt : memory[curPage][k2])
			{
				bl.push_back(bt);
			}
			for (byte bt : memory[curPage][k1])
			{
				bl.push_back(bt);
			}
			memory[curPage][k2] = bl;
			break;
		case opcode::APPENDB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			bl = vector<byte>();
			for (byte bt : memory[curPage][k2])
			{
				bl.push_back(bt);
			}
			bl.push_back(memory[curPage][k1][0]);
			if (memory[curPage].count(k2)) memory[curPage][k2] = bl;
			else memory[curPage].insert({ k2, bl });
			break;
		case opcode::CLRB:
			k = i.parameters[0];
			memory[curPage].erase(k);
			break;
		case opcode::VAC:
			k1 = bitconverter::toint32(i.parameters, 0);
			ii = 0;
			while (arrays.find(ii) != arrays.end())
			{
				ii++;
			}
			arrays.insert({ ii, arrayobj() });
			memory[curPage][k1] = bitconverter::toarray(ii);
			break;
		case opcode::VAD:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			arrays[k1].data.push_back(memory[curPage][k2]);
			break;
		case opcode::VAR:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			arrays[k1].data.erase(arrays[k1].data.begin() + k2);
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
			arrays[k2].data.insert(arrays[k2].data.end(), arrays[k1].data.begin(), arrays[k1].data.end());
			break;
		case opcode::VPF:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			arrays[k1].data.push_front(memory[curPage][k2]);
			break;
		case opcode::VADI:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			v = i.parameters;
			v.erase(v.begin(), v.begin() + 4);
			arrays[k1].data.push_back(v);
			break;
		case opcode::VAL:
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0);
			k2 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0);
			memory[curPage][k2] = bitconverter::toarray(arrays[bitconverter::toint32(memory[curPage][k1], 0)].data.size());
			break;
		case opcode::VAS:
			k = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 0)], 0); // indexand
			k1 = bitconverter::toint32(memory[curPage][bitconverter::toint32(i.parameters, 4)], 0); // index
			k2 = bitconverter::toint32(i.parameters, 8); // source
			arrays[k].data[k1] = memory[curPage][k2];
			break;
		case opcode::MOVL:
			k = bitconverter::toint32(i.parameters, 0);
			v = i.parameters;
			v.erase(v.begin(), v.begin() + 4);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].insert({ k, v });
			break;
		case opcode::ADD:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) + bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::SUB:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) - bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::MUL:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) * bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::DIV:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			memory[curPage][k2] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k1], 0) / bitconverter::toint32(memory[curPage][k2], 0));
			break;
		case opcode::INC:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k], 0) + bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::DEC:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k], 0) - bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::IMUL:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k], 0) * bitconverter::toint32(i.parameters, 4));
			break;
		case opcode::IDIV:
			k = bitconverter::toint32(i.parameters, 0);
			memory[curPage][k] = bitconverter::toarray(bitconverter::toint32(memory[curPage][k], 0) / bitconverter::toint32(i.parameters, 4));
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
			if (v1.size() == i.parameters.size() - 4)
			{
				for (n = 4; n < i.parameters.size(); n++)
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
			for (pair<int, pair<int, int>> p : pages)
			{
				if (p.second.first <= pc && p.second.second >= pc) curPage = p.first;
			}
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
			if (callstack.size() == 0)
			{
				halt();
			}
			else
			{
				pc = callstack.back();
				callstack.pop_back();
				for (pair<int, pair<int, int>> p : pages)
				{
					if (p.second.first <= pc && p.second.second >= pc) curPage = p.first;
				}
			}
			break;
		case opcode::EMIT:
			k = bitconverter::toint32(i.parameters, 0);
			v = memory[curPage][k];
			emc = disassembler::disassembleExecutable(v);
			addr = bytecode.size();
			for (instruction h : emc) bytecode.push_back(h);
			branch(addr);
			break;
		case opcode::MOVPC:
			k = bitconverter::toint32(i.parameters, 0);
			v = bitconverter::toarray(pc);
			if (memory[curPage].count(k)) memory[curPage][k] = v;
			else memory[curPage].insert({ k, v });
			break;
		case opcode::LJ:
			pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::SJ:
			branch((int)i.parameters[0]);
			break;
		case opcode::INTS:
			v = syscall::systemCall(i.parameters, this);
			if (v.size() != 0) astack.push(v);
			break;
		case opcode::INTR:
			inter = i.parameters[0];
			k = bitconverter::toint32(i.parameters, 1);
			v = memory[curPage][k];
			v.insert(v.begin(), inter);
			v = syscall::systemCall(v, this);
			if (v.size() != 0) astack.push(v);
			break;
		case opcode::BREAK:
			// TODO: inform debugger?
			break;
		case opcode::INTB:
			inter = i.parameters[0];
			k = i.parameters[1];
			v = memory[curPage][k];
			v.insert(v.begin(), inter);
			v = syscall::systemCall(v, this);
			if (v.size() != 0) astack.push(v);
			break;
		case opcode::BITS:
			bits = i.parameters[0];
			break;
		case opcode::PUSH:
			if (bits == 32)
				astack.push(memory[curPage][bitconverter::toint32(i.parameters, 0)]);
			else if (bits == 8)
				astack.push({ memory[curPage][bitconverter::toint32(i.parameters, 0)][0] });
			break;
		case opcode::POP:
			if (astack.size() != 0)
			{
				k = bitconverter::toint32(i.parameters, 0);
				if (memory[curPage].count(k)) memory[curPage][k] = astack.top();
				else memory[curPage].insert({ k, astack.top() });
				if (bits == 8)
				{
					v = { 0, 0, 0 };
					memory[curPage][k].insert(memory[curPage][k].end(), v.begin(), v.end());
				}
				astack.pop();
			}
			break;
		case opcode::SPUSH:
			if (bits == 32)
				astack.push(i.parameters);
			else if (bits == 8)
				astack.push({ i.parameters[0] });
			break;
		case opcode::POPA:
			astack.empty();
			break;
		case opcode::SPOP:
			astack.pop();
			break;
		case opcode::POPB:
			if (astack.size() != 0)
			{
				k = (int)i.parameters[0];
				if (memory[curPage].count(k)) memory[curPage][k] = astack.top();
				else memory[curPage].insert({ k, astack.top() });
				if (bits == 8)
				{
					v = { 0, 0, 0 };
					memory[curPage][k].insert(memory[curPage][k].end(), v.begin(), v.end());
				}
				astack.pop();
			}
			break;
		case opcode::VPUSHB:
			if (bits == 32)
				astack.push(memory[curPage][i.parameters[0]]);
			else if (bits == 8)
				astack.push({ memory[curPage][i.parameters[0]][0] });
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
		if (interm->input.size() > 0)
		{
			if (interm->input[interm->input.size() - 1] == '\n')
			{
				interm->input.erase(interm->input.end() - 1);
				astack.push(interm->input);
				interm->input = vector<byte>();
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
	callstack.push_back(pc);
	pc = addr;
}
void nvm::halt()
{
	running = false;
}
void nvm::halt(string err)
{
	running = false;
}