#include "nvm.h"
#include "disassembler.h"
#include "syscall.h"
#include "bitconverter.h"
#include "event.h"
#include "dynamiclinker.h"
#include "memorystats.h"
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
	bits = 32;
	awaitin = false;
	waitForProcInput = -1;
	millis = 0;
	arrays = map<int, arrayobj>();
	memory = IntMap<vmobject>();
	interm = new vt(processid, -1, vtype::StandardInput);
	outterm = new vt(processid, -1, vtype::StandardOutput);
	globalPages = Array<vmobject>();
	globalPages.add(vmobject());
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
	processPriority = 1;
	suspended = true;
	eventsenabled = false;
	extcalls = map<int, int>();
	modules = map<string, pair<int, int>>();
	lnkndx = 0;
	bits = 32;
	waitForProcInput = -1;
	awaitin = false;
	millis = 0;
	arrays = map<int, arrayobj>();
	interm = new vt(processid, -1, vtype::StandardInput);
	outterm = new vt(processid, -1, vtype::StandardOutput);
	memory = IntMap<vmobject>();
	globalPages = Array<vmobject>();
	globalPages.add(vmobject());
}
void nvm::start()
{
	dynamiclinker::dynamicLink(this);
	globals = &globalPages[0];
	running = true;
	suspended = false;
	eventsenabled = true;
	bits = 32;
	millis = ntime::getMillis();
}
void nvm::start(int procid, string file)
{
	fileName = file;
	processid = procid;
	start();
}
void nvm::cycle()
{
	for(int pr = 0; pr < processPriority; pr++)
	if (running && !awaitmsg && !awaitin && waitForProcInput == -1)
	{
		if (pc >= bytecode->size)
		{
			running = false;
			return;
		}
		i = bytecode->get(pc);
		//cout << "OpCode " << i.opCode << endl;
		pc += 1;
		switch (i.opCode)
		{
		case opcode::NOP:
			break;
		case opcode::DELFLD:
			k = bitconverter::toint32(astack.getTop());
			astack.pop();
			memory[k].remove(bitconverter::toint32(astack.getTop()));
			astack.pop();
			break;
		case opcode::AND:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			globals->set(k2, bitconverter::toArray(bitconverter::toint32(globals->get(k1), 0) & bitconverter::toint32(globals->get(k2), 0)));
			break;
		case opcode::OR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			globals->set(k2, bitconverter::toArray(bitconverter::toint32(globals->get(k1), 0) | bitconverter::toint32(globals->get(k2), 0)));
			break;
		case opcode::XOR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			globals->set(k2, bitconverter::toArray(bitconverter::toint32(globals->get(k1), 0) ^ bitconverter::toint32(globals->get(k2), 0)));
			break;
		case opcode::SHL:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			globals->set(k2, bitconverter::toArray(bitconverter::toint32(globals->get(k1), 0) << bitconverter::toint32(globals->get(k2), 0)));
			break;
		case opcode::SHR:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			globals->set(k2, bitconverter::toArray(bitconverter::toint32(globals->get(k1), 0) >> bitconverter::toint32(globals->get(k2), 0)));
			break;
		case opcode::NOT:
			k1 = bitconverter::toint32(globals->get(bitconverter::toint32(i.parameters, 0)), 0);
			globals->set(bitconverter::toint32(i.parameters, 0), bitconverter::toArray(~k1));
			break;
		case opcode::ST:
			k = bitconverter::toint32(i.parameters, 0);
			v = Array<byte>(i.parameters, i.psize);
			v.erase(0, 4);
			globals->set(k, v);
			break;
		case opcode::TOSTR:
			v = astack.getTop();
			astack.pop();
			astack.push(bitconverter::toArray(to_string(bitconverter::toint32(v))));
			break;
		case opcode::PARSEINT:
			v = astack.getTop();
			astack.pop();
			astack.push(bitconverter::toArray(stoi(bitconverter::tostring(v))));
			break;
		case opcode::CLR:
			globals->set(bitconverter::toint32(i.parameters, 0), Array<byte>());
			break;
		case opcode::CONCAT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			bl = globals->get(k2);
			for (n = 0; n < globals->get(k1).size; n++)
			{
				bl.push(globals->get(k1)[n]);
			}
			globals->set(k2, bl);
			break;
		case opcode::MOV:
			globals->set(bitconverter::toint32(i.parameters, 4), globals->get(bitconverter::toint32(i.parameters, 0)));
			break;
		case opcode::SPLIT:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			k = bitconverter::toint32(i.parameters, 8);
			po = globals->get(k1);
			t = Array<byte>();
			pu = Array<byte>();
			for (n = 0; n < po.size; n++)
			{
				if (n < (unsigned int)k) t.push(po[n]);
				else pu.push(po[n]);
			}
			globals->set(k1, t);
			globals->set(k2, pu);
			break;
		case opcode::INDEX:
			k1 = bitconverter::toint32(globals->get(bitconverter::toint32(i.parameters, 4)), 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			if (bits == 32)
				globals->set(k2, bitconverter::toArray((int)globals->get(bitconverter::toint32(i.parameters, 0))[k1]));
			else if (bits == 8)
			{
				po = Array<byte>();
				po.add(globals->get(bitconverter::toint32(i.parameters, 0))[k1]);
				globals->set(k2, Array<byte>(po));
				po.clear();
			}
			break;
		case opcode::INSERT:
			k1 = bitconverter::toint32(globals->get(bitconverter::toint32(i.parameters, 4)), 0);
			k2 = bitconverter::toint32(i.parameters, 8);
			v = globals->get(bitconverter::toint32(i.parameters, 0));
			v[k1] = globals->get(k2)[0];
			globals->set(bitconverter::toint32(i.parameters, 0), v);
			break;
		case opcode::SIZ:
			v = globals->get(bitconverter::toint32(i.parameters, 0));
			k1 = bitconverter::toint32(i.parameters, 4);
			globals->set(k1, bitconverter::toArray(v.size));
			break;
		case opcode::APPEND:
			k = bitconverter::toint32(i.parameters, 4);
			bl = globals->get(k);
			bl.push(globals->get(bitconverter::toint32(i.parameters, 0))[0]);
			globals->set(bitconverter::toint32(i.parameters, 4), bl);
			break;
		case opcode::NEWOBJ:
			ii = 0;
			while (memory.find(ii))
			{
				ii++;
			}
			memory.add(ii, vmobject());
			memory.get(ii);
			astack.push(bitconverter::toArray(ii));
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
			globals->set(k, v);
			break;
		case opcode::PUSHB:
			astack.push(bitconverter::toArray((int)i.parameters[0]));
			break;
		case opcode::CONCATB:
			k1 = i.parameters[0];
			k2 = i.parameters[1];
			bl = globals->get(k2);
			for (n = 0; n < globals->get(k1).size; n++)
			{
				bl.push(globals->get(k1)[n]);
			}
			globals->set(k2, bl);
			break;
		case opcode::APPENDB:
			k = i.parameters[1];
			bl = globals->get(k);
			bl.push(globals->get(i.parameters[0])[0]);
			globals->set(k, bl);
			break;
		case opcode::CLRB:
			globals->set(i.parameters[0], Array<byte>());
			break;
		case opcode::VAC:
			ii = 0;
			while (arrays.find(ii) != arrays.end())
			{
				ii++;
			}
			arrays.emplace(ii, arrayobj());
			astack.push(bitconverter::toArray(ii));
			break;
		case opcode::VAD:
			k1 = bitconverter::toint32(astack.getTop());
			astack.pop();
			arrays[k1].data.push(astack.getTop());
			astack.pop();
			break;
		case opcode::DELELEM:
			k1 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			k2 = bitconverter::toint32(astack.getTop()); // array
			astack.pop();
			arrays[k2].data.removeAt(k1);
			break;
		case opcode::LDELEM:
			k1 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			k2 = bitconverter::toint32(astack.getTop()); // array
			astack.pop();
			astack.push(arrays[k2].data[k1]);
			break;
		case opcode::VADE:
			k = bitconverter::toint32(astack.getTop());
			astack.pop();
			arrays.erase(k);
			break;
		case opcode::LDFLD:
			k1 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			k2 = bitconverter::toint32(astack.getTop()); // object
			astack.pop();
			astack.push(memory[k2].get(k1));
			break;
		case opcode::STFLD:
			k1 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			k2 = bitconverter::toint32(astack.getTop()); // object
			astack.pop();
			memory[k2].set(k1, astack.getTop());
			astack.pop();
			break;
		case opcode::SWAP:
			v = astack.getTop();
			astack.pop();
			v1 = astack.getTop();
			astack.pop();
			astack.push(v);
			astack.push(v1);
			break;
		case opcode::LDLEN:
			k = bitconverter::toint32(astack.getTop());
			astack.pop();
			astack.push(bitconverter::toArray(arrays[k].data.size));
			break;
		case opcode::STELEM:
			k1 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			k2 = bitconverter::toint32(astack.getTop()); // index
			astack.pop();
			arrays[k2].data[k1] = astack.getTop();
			astack.pop();
			break;
		case opcode::PUSHL:
			astack.push(Array<byte>(i.parameters, 4));
			break;
		case opcode::ADD:
			if (astack.size > 1)
			{
				k1 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				k2 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				astack.push(bitconverter::toArray(k2 + k1));
			}
			else vmmgr::vmmerror("Stack underflow at " + pc, processid);
			break;
		case opcode::SUB:
			if (astack.size > 1)
			{
				k1 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				k2 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				astack.push(bitconverter::toArray(k2 - k1));
			}
			else vmmgr::vmmerror("Stack underflow at " + pc, processid);
			break;
		case opcode::MUL:
			if (astack.size > 1)
			{
				k1 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				k2 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				astack.push(bitconverter::toArray(k2 * k1));
			}
			else vmmgr::vmmerror("Stack underflow at " + pc, processid);
			break;
		case opcode::DIV:
			if (astack.size > 1)
			{
				k1 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				k2 = bitconverter::toint32(astack.getTop(), 0);
				astack.pop();
				astack.push(bitconverter::toArray(k2 / k1));
			}
			else vmmgr::vmmerror("Stack underflow at " + pc, processid);
			break;
		case opcode::INC:
			k = bitconverter::toint32(i.parameters, 0);
			globals->set(k, bitconverter::toArray(bitconverter::toint32(globals->get(k), 0) + bitconverter::toint32(i.parameters, 4)));
			break;
		case opcode::DEC:
			k = bitconverter::toint32(i.parameters, 0);
			globals->set(k, bitconverter::toArray(bitconverter::toint32(globals->get(k), 0) - bitconverter::toint32(i.parameters, 4)));
			break;
		case opcode::IMUL:
			k = bitconverter::toint32(i.parameters, 0);
			globals->set(k, bitconverter::toArray(bitconverter::toint32(globals->get(k), 0) * bitconverter::toint32(i.parameters, 4)));
			break;
		case opcode::IDIV:
			k = bitconverter::toint32(i.parameters, 0);
			globals->set(k, bitconverter::toArray(bitconverter::toint32(globals->get(k), 0) / bitconverter::toint32(i.parameters, 4)));
			break;
		case opcode::SCMP:
			if (astack.size < 2) vmmgr::vmmerror("Stack underflow at " + pc, processid);
			cv1 = bitconverter::toint32(astack.getTop(), 0);
			astack.pop();
			cv2 = bitconverter::toint32(astack.getTop(), 0);
			astack.pop();
			less = false;
			greater = false;
			equal = false;
			zero = false;
			if (cv1 < cv2) less = true;
			if (cv1 > cv2) greater = true;
			if (cv1 == cv2) equal = true;
			break;
		case opcode::CMP:
			k1 = bitconverter::toint32(i.parameters, 0);
			k2 = bitconverter::toint32(i.parameters, 4);
			cv1 = bitconverter::toint32(globals->get(k1), 0);
			cv2 = bitconverter::toint32(globals->get(k2), 0);
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
			cv1 = bitconverter::toint32(globals->get(k1), 0);
			cv2 = bitconverter::toint32(globals->get(k2), 0);
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
			k2 = bitconverter::toint32(globals->get(k1), 0);
			zero = false;
			if (k2 == 0) zero = true;
			break;
		case opcode::CZB:
			k1 = i.parameters[0];
			k2 = bitconverter::toint32(globals->get(k1), 0);
			zero = false;
			if (k2 == 0) zero = true;
			break;
		case opcode::CMPS:
			less = false;
			greater = false;
			equal = true;
			zero = false;
			v1 = globals->get(bitconverter::toint32(i.parameters, 0));
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
			cmpi = bitconverter::toint32(globals->get(bitconverter::toint32(i.parameters, 0)), 0);
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
			cmpi = bitconverter::toint32(globals->get(i.parameters[0]), 0);
			cmpid = i.parameters[1];
			if (cmpi == cmpid) equal = true;
			if (cmpi < cmpid) less = true;
			if (cmpi > cmpid) greater = true;
			break;
		case opcode::JMP:
			branch(bitconverter::toint32(i.parameters, 0));
			break;
		case opcode::LEAP:
			leap(bitconverter::toint32(i.parameters, 0), i.parameters[4]);
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
					if (p.second.first <= pc && p.second.second >= pc)
					{
						globals = &globalPages[p.first];
					}
				}
			}
			break;
		case opcode::EMIT:
			k = bitconverter::toint32(i.parameters, 0);
			v = globals->get(k);
			emc = *disassembler::disassembleCode(v.holder, v.size);
			addr = bytecode->size;
			for (n = 0; n < emc.size; n++) bytecode->push(emc[n]);
			branch(addr);
			break;
		case opcode::MOVPC:
			globals->set(bitconverter::toint32(i.parameters, 0), bitconverter::toArray(pc));
			break;
		case opcode::LJ:
			pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJE:
			if (equal) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJNE:
			if (!equal) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJG:
			if (greater) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJL:
			if (less) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJGE:
			if (equal || greater) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::LJLE:
			if (equal || less) pc = bitconverter::toint32(i.parameters, 0);
			break;
		case opcode::JSP:
			branch(bitconverter::toint32(astack.getTop()));
			astack.pop();
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
			v = globals->get(k);
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
			v = globals->get(k);
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
				astack.push(globals->get(bitconverter::toint32(i.parameters, 0)));
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(globals->get(bitconverter::toint32(i.parameters, 0))[0]);
				astack.push(v);
			}
			break;
		case opcode::POP:
			if (astack.size != 0)
			{
				globals->set(bitconverter::toint32(i.parameters, 0), astack.getTop());
				if (bits == 8)
				{
					v = Array<byte>();
					v.push(0); v.push(0); v.push(0);
					k = bitconverter::toint32(i.parameters, 0);
					v.add(globals->get(k)[0]);
					globals->set(k, v);
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
				globals->set(i.parameters[0], astack.getTop());
				if (bits == 8)
				{
					v = Array<byte>();
					v.push(0); v.push(0); v.push(0);
					k = (int)i.parameters[0];
					v.add(globals->get(k)[0]);
					globals->set(k, v);
				}
				astack.pop();
			}
			break;
		case opcode::VPUSHB:
			if (bits == 32)
				astack.push(globals->get(i.parameters[0]));
			else if (bits == 8)
			{
				v = Array<byte>();
				v.push(globals->get(i.parameters[0])[0]);
				astack.push(v);
			}
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
}

void nvm::leap(int addr, byte page)
{
	callstack.push(pc);
	pc = addr;
	for (pair<int, pair<int, int>> p : pages)
	{
		if (p.second.first <= pc && p.second.second >= pc)
		{
			globals = &globalPages[p.first];
			return;
		}
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

void nvm::setTerminals(vt in, vt out)
{
	*interm = in;
	*outterm = out;
}
