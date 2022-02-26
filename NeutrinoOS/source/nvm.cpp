#include "nvm.h"
#include "disassembler.h"
#include "syscall.h"
#include "bitconverter.h"
#include "event.h"
#include "dynamiclinker.h"
#include "memorystats.h"
#include "kernlog.h"

#define INT(b) bitconverter::toint32(b)
#define INTE(b, o) bitconverter::toint32(b, o)
#define STRING(b) bitconverter::tostring(b)
#define BYTEI(i) bitconverter::toArray(i)
#define BYTES(s) bitconverter::toArray(s)
#define PARAMI(o) bitconverter::toint32(i.parameters, 0)
#define PARAMS(o, s) bitconverter::tostring(i.parameters, o, s)
#define PUSHNEW(o) astack.push(newobj(o))
#define PUSHVAL(v) astack.push(newobj(vmobject(v)))
#define STACKTOP() memory.get(astack.getTop())
#define DECREF(o) { memory.get(o)->refcount -= 1; if(memory.get(o)->refcount <= 0) memory.remove(o); }

nvm::nvm()
{
	initialize();
}

nvm::nvm(Array<instruction>* code)
{
	initialize();
	bytecode = code;
}

nvm::~nvm()
{
	for(int i = 0; i < bytecode->size; i++) delete[] (*bytecode)[i].parameters;
	bytecode->clear();
	delete bytecode;
	globalPages.clear();
	localScopes.clear();
	arrays.clear();
	pages.clear();
	eventHandlers.clear();
	timers.clear();
	astack.clear();
	callstack.clear();
	messages.clear();
	delete interm;
	delete outterm;
	v.clear();
	v1.clear();
	bl.clear();
	t.clear();
	pu.clear();
	po.clear();
	arr1.clear();
	arr2.clear();
	emc.clear();
	eventQueue.clear();
	timerQueue.clear();
}

void nvm::initialize()
{
    //memory = ObjectMap();
	flagstack = Array<cflags>();
	flagstack.push({ false, false, false });
	fileName = "??";
	eventHandlers = map<byte, int>();
	eventQueue = Array<ntrevent>();
	timerQueue = Array<ntrevent>();
	astack.top = -1;
	pc = 0;
	running = false;
	awaitmsg = false;
	processid = 0;
	inhandler = -1;
	processPriority = 1;
	suspended = true;
	eventsenabled = false;
	eventsuspended = false;
	lnkndx = 0;
	waitForProcInput = -1;
	awaitin = false;
	millis = 0;
	arrays = map<int, arrayobj>();
	interm = new vt(processid, -1, vtype::StandardInput);
	outterm = new vt(processid, -1, vtype::StandardOutput);
	globalPages = Array<vmobject>();
	globalPages.add(vmobject());
	localScopes = Array<Array<vmobject>>();
	localScopes.add(Array<vmobject>());
	localScopes.get(0).add(vmobject());
	localScopes.get(0).get(0).refcount = 1;
	currentScopes = Array<Array<int>>();
	curPage = 0;
}

void nvm::start()
{
	dynamiclinker::dynamicLink(this);
	globals = &(globalPages.get(0));
	locals = &(localScopes.get(0).get(0));
	running = true;
	suspended = false;
	eventsenabled = true;
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
		pc += 1;
		int ival;
		vmobject* vo;
		int vadr;
		switch (i.opCode)
		{
		case opcode::NOP:
			break;
		case opcode::DELFLD:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = astack.getTop(); // object
			astack.pop();
			memory.get(k2)->remove(k1);
			break;
		case opcode::LDLOC:
			astack.push(locals->get(PARAMI(0)));
			break;
		case opcode::STLOC:
			k = astack.getTop();
			astack.pop();
			k1 = PARAMI(0);
			memory.get(k)->refcount += 1;
			if(locals->find(k1)) DECREF(locals->get(k1));
			locals->set(k1, k);
			break;
		case opcode::SWSCOP:
			currentScopes[curPage].push(PARAMI(0));
			locals = &(localScopes.get(curPage).getSafe(currentScopes[curPage].getTop()));
			locals->refcount = 1;
			break;
		case opcode::NOT:
			k = PARAMI(0);
			memory.get(k)->setValue(~(memory.get(k)->getValue()));
			break;
		case opcode::TOSTR:
			k = astack.getTop();
			astack.pop();
			PUSHVAL(to_string(memory.get(k)->getValue()));
			break;
		case opcode::PARSEINT:
			k = astack.getTop();
			astack.pop();
			PUSHVAL(stoi(STRING(memory.get(k)->getValue())));
			astack.pop();
			break;
		case opcode::CLR:
			if (memory.find(astack.getTop()))
			{
				memory.remove(astack.getTop());
				astack.pop();
			}
			break;
		case opcode::INDEX:
			k = STACKTOP()->getValue();  // index
			astack.pop();
			bp = STACKTOP()->boundValue; // source
			astack.pop();
			PUSHVAL((int)v1[k]);
			break;
		case opcode::NEWOBJ:
			astack.push(newobj(vmobject()));
			break;
		case opcode::VAC:
			ii = 0;
			while (arrays.find(ii) != arrays.end())
			{
				ii++;
			}
			arrays.emplace(ii, arrayobj());
			astack.push(newobj(vmobject(ii, DefaultType::ArrayPointer)));
			break;
		case opcode::VAD:
			k1 = astack.getTop();
			astack.pop();
			arrays[k1].data.push(astack.getTop());
			astack.pop();
			break;
		case opcode::DELELEM:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = astack.getTop(); // array
			astack.pop();
			arrays[k2].data.removeAt(k1);
			break;
		case opcode::LDELEM:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = STACKTOP()->getValue(); // array
			astack.pop();
			astack.push(newobj(vmobject(arrays[k2].data[k1])));
			break;
		case opcode::VADE:
			k = STACKTOP()->getValue();
			astack.pop();
			arrays.erase(k);
			break;
		case opcode::LDFLD:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = astack.getTop(); // object
			astack.pop();
			astack.push(memory.get(k2)->get(k1));
			break;
		case opcode::STFLD:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = astack.getTop(); // object
			astack.pop();
			if(memory.get(k2)->find(k1)) DECREF(memory.get(k2)->get(k1));
			memory.get(k2)->set(k1, astack.getTop());
			memory.get(astack.getTop())->refcount += 1;
			astack.pop();
			break;
		case opcode::SWAP:
			k = astack.getTop();
			astack.pop();
			k1 = astack.getTop();
			astack.pop();
			astack.push(k);
			astack.push(k1);
			break;
		case opcode::LDLEN:
			k = astack.getTop();
			astack.pop();
			k1 = memory.get(k)->type;
			if (k1 == DefaultType::Int) PUSHVAL(4);
			else if (k1 == DefaultType::String) PUSHVAL(memory.get(k)->bsize);
			else if (k1 == DefaultType::ArrayPointer) PUSHVAL(arrays[memory.get(k)->getValue()].data.size);
			astack.pop();
			PUSHVAL(arrays[k].data.size);
			break;
		case opcode::STELEM:
			k1 = STACKTOP()->getValue(); // index
			astack.pop();
			k2 = STACKTOP()->getValue(); // array
			astack.pop();
			arrays[k2].data[k1] = astack.getTop();
			astack.pop();
			break;
		case opcode::PUSHL:
			PUSHVAL(PARAMI(0));
			break;
		case opcode::ADD:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 0));
			break;
		case opcode::SUB:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 1));
			break;
		case opcode::MUL:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 2));
			break;
		case opcode::DIV:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 3));
			break;
		case opcode::AND:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 4));
			break;
		case opcode::OR:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 5));
			break;
		case opcode::XOR:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 6));
			break;
		case opcode::SHL:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 7));
			break;
		case opcode::SHR:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 8));
			break;
		case opcode::PWR:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 9));
			break;
		case opcode::MOD:
			k1 = astack.getTop();
			astack.pop();
			k2 = astack.getTop();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory.get(k2), memory.get(k1), 10));
			break;
		case opcode::CMP:
			cv1 = STACKTOP()->getValue();
			astack.pop();
			cv2 = STACKTOP()->getValue();
			astack.pop();
			//                      less       greater    equal
			//                  TOS-1 < TOS TOS-1 > TOS  TOS-1 == TOS
			flagstack.getTop() = { cv1 > cv2, cv1 < cv2, cv1 == cv2 };
			break;
		case opcode::RET:
			ret();
			break;
		case opcode::IRET:
			iret();
			break;
		case opcode::EMIT:
			k = PARAMI(0);
			v = globals->get(k);
			emc = *disassembler::disassembleCode(v.holder, v.size);
			addr = bytecode->size;
			for (n = 0; n < emc.size; n++) bytecode->push(emc[n]);
			branch(addr);
			break;
		case opcode::DUP:
			astack.push(astack.getTop());
			break;
		case opcode::LEAP:
			leap(STACKTOP()->getValue());
			astack.pop();
			break;
		case opcode::JMP:
			pc = PARAMI(0);
			break;
		case opcode::BR:
			branch(PARAMI(0));
			break;
		case opcode::BRE:
			branch(STACKTOP()->getValue());
			astack.pop();
			break;
		case opcode::IFEQ:
			if (!flagstack.getTop().equal) pc += 1;
			break;
		case opcode::IFNE:
			if (flagstack.getTop().equal) pc += 1;
			break;
		case opcode::IFLE:
			if (!flagstack.getTop().equal && flagstack.getTop().greater) pc += 1;
			break;
		case opcode::IFGE:
			if (!flagstack.getTop().equal && flagstack.getTop().less) pc += 1;
			break;
		case opcode::IFLT:
			if (flagstack.getTop().greater || flagstack.getTop().equal) pc += 1;
			break;
		case opcode::IFGT:
			if (flagstack.getTop().less || flagstack.getTop().equal) pc += 1;
			break;
		case opcode::BREAK:
			// TODO: inform debugger?
			klog("NVM", "Breakpoint");
			break;
		case opcode::GC:
			//if(astack.top > -1) memory.get(astack.getTop())->refcount++;
			trash();
			break;
		case opcode::INTR:
			inter = i.parameters[0];
			v = Array<byte>(STACKTOP()->boundValue, STACKTOP()->bsize);
			astack.pop();
			v.insert(0, inter);
			bp = bitconverter::toarray_p(v);
			vmmgr::enterCriticalSection();
			v = syscall::systemCall(bp, v.size, this);
			vmmgr::leaveCriticalSection();
			if (v.size != 0) astack.push(newobj(vmobject(v)));
			delete[] bp;
			break;
		case opcode::LDGL:
			astack.push(globals->get(PARAMI(0)));
			break;
		case opcode::STGL:
			k = astack.getTop();
			astack.pop();
			k1 = PARAMI(0);
			memory.get(k)->refcount += 1;
			if(globals->find(k1)) DECREF(globals->get(k1));
			globals->set(k1, k);
			break;
		case opcode::LDSTR:
			PUSHVAL(vmobject(Array<byte>(i.parameters, i.psize)));
			// memory.get(astack.getTop())->type = DefaultType::String;
			// not needed because it is String by default.
			break;
		case opcode::LDI:
			PUSHVAL(PARAMI(0));
			memory.get(astack.getTop())->type = DefaultType::Int;
			break;
		case opcode::LDB:
			PUSHVAL(Array<byte>(i.parameters, 1));
			memory.get(astack.getTop())->type = DefaultType::Byte;
			break;
	    case opcode::TOP:
	        astack.moveTop(PARAMI(0));
			break;
		case opcode::SPOP:
			astack.pop();
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
				astack.push(newobj(vmobject(interm->input)));
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

int nvm::newobj(vmobject o)
{
	ii = 0;
	while (memory.find(ii))
	{
		ii++;
	}
	memory.add(ii, o);
	return ii;
}

void nvm::processEvents()
{
	if (eventQueue.size > 0)
	{
		int eix = eventQueue.size - 1;
		inhandler = pc;
		branch(eventHandlers[eventQueue[eix].id]);
		eventQueue.removeAt(eix);
		eventsenabled = false;
		suspended = false;
	}
	if (timerQueue.size > 0)
	{
		int eix = timerQueue.size - 1;
		inhandler = pc;
		branch(timers[timerQueue[eix].id].handler);
		timerQueue.removeAt(eix);
		eventsenabled = false;
		suspended = false;
	}
}

void nvm::branch(int addr)
{
	callstack.push(pc);
	pc = addr;
	flagstack.push({ false, false, false });
}

void nvm::iret()
{
	pc = callstack.getTop();
	callstack.pop();
	flagstack.pop();
}

void nvm::ret()
{
	if (callstack.size == 0)
	{
		halt();
	}
	else
	{
		pc = callstack.getTop();
		callstack.pop();
		flagstack.pop();
		currentScopes[curPage].pop();
		for (pair<int, pair<int, int>> p : pages)
		{
			if (p.second.first <= pc && p.second.second >= pc)
			{
				curPage = p.first;
				globals = &(globalPages.get(curPage));
				locals = &(localScopes.get(curPage)[currentScopes.get(curPage).getTop()]);
			}
		}
		if (pc == inhandler)
		{
			inhandler = -1;
			eventsenabled = true;
		}
	}
}

void nvm::leap(int addr)
{
	callstack.push(pc);
	pc = addr;
	flagstack.push({ false, false, false });
	for (pair<int, pair<int, int>> p : pages)
	{
		if (p.second.first <= pc && p.second.second >= pc)
		{
			curPage = p.first;
			globals = &(globalPages.get(curPage));
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

void nvm::trash()
{
	int ign = -1;
	if(astack.top > -1)
		ign = astack.getTop();
	int rc = 0;
	for(int i = 0; i < locals->count; i++)
	{
		if(locals->keys[i] != -1)
		{
			rc = memory.get(locals->get(i))->refcount;
			if(memory.get(locals->get(i))->refcount <= 1)
			{
				if(locals->get(i) == ign) memory.get(ign)->refcount = 0;
				else memory.remove(locals->get(i));
			}
			locals->remove(i);
		}
	}
}
