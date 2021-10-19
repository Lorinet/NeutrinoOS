#include "nvm.h"
#include "disassembler.h"
#include "syscall.h"
#include "bitconverter.h"
#include "event.h"
#include "dynamiclinker.h"
#include "memorystats.h"

#define INT(b) bitconverter::toint32(b)
#define INTE(b, o) bitconverter::toint32(b, o)
#define STRING(b) bitconverter::tostring(b)
#define BYTEI(i) bitconverter::toArray(i)
#define BYTES(s) bitconverter::toArray(s)
#define PARAMI(o) bitconverter::toint32(i.parameters, 0)
#define PARAMS(o, s) bitconverter::tostring(i.parameters, o, s)
#define STACKI() bitconverter::toint32(astack.getTop())
#define PUSHNEW(o) astack.push(newobj(o))
#define PUSHVAL(v) astack.push(newobj(vmobject(v)))
#define STACKTOP() memory->get(astack.getTop())

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
	delete memory;
	globalPages->clear();
	pageAddresses->clear();
	localScopes->clear();
	delete globalPages;
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
}

void nvm::initialize()
{
	fileName = "??";
	eventHandlers = map<byte, int>();
	eventQueue = Array<ntrevent>();
	timerQueue = Array<ntrevent>();
	astack.top = -1;
	pc = 0;
	running = false;
	equal = false;
	less = false;
	greater = false;
	zero = false;
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
	memory = new IntMap<vmobject>();
	globalPages = new Array<vmobject>();
	globalPages->add(vmobject());
	pageAddresses = new Array<int>();
	localScopes = new Array<Array<vmobject>>();
	localScopes->add(Array<vmobject>());
	localScopes->get(0).add(vmobject());
	currentScopes.add(0);
	curPage = 0;
}

void nvm::start()
{
	dynamiclinker::dynamicLink(this);
	globals = &(globalPages->get(0));
	locals = &(localScopes->get(0).get(0));
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
		switch (i.opCode)
		{
		case opcode::NOP:
			break;
		case opcode::DELFLD:
			k = STACKI();
			astack.pop();
			memory->get(k).remove(STACKI());
			astack.pop();
			break;
		case opcode::LDLOC:
			astack.push(locals->get(PARAMI(0)));
			break;
		case opcode::STLOC:
			if (astack.size != 0)
			{
				locals->set(PARAMI(0), astack.getTop());
				astack.pop();
			}
			break;
		case opcode::SWSCOP:
			currentScopes[curPage] = PARAMI(0);
			break;
		case opcode::NOT:
			k = PARAMI(0);
			memory->get(k).setValue(~memory->get(k).getValue());
			break;
		case opcode::TOSTR:
			PUSHVAL(to_string(INT(STACKTOP().getValue())));
			astack.pop();
			break;
		case opcode::PARSEINT:
			PUSHVAL(stoi(STRING(STACKTOP().getValue())));
			astack.pop();
			break;
		case opcode::CLR:
			if (memory->find(astack.getTop()))
			{
				memory->remove(astack.getTop());
				astack.pop();
			}
			break;
		case opcode::INDEX:
			k = STACKTOP().getValue();  // index
			astack.pop();
			bp = STACKTOP().boundValue; // source
			astack.pop();
			k2 = astack.getTop();       // destination
			astack.pop();
			memory->set(k2, vmobject((int)v1[k]));
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
			k1 = STACKI();
			astack.pop();
			arrays[k1].data.push(astack.getTop());
			astack.pop();
			break;
		case opcode::DELELEM:
			k1 = STACKTOP().getValue(); // index
			astack.pop();
			k2 = STACKI(); // array
			astack.pop();
			arrays[k2].data.removeAt(k1);
			break;
		case opcode::LDELEM:
			k1 = STACKTOP().getValue(); // index
			astack.pop();
			k2 = STACKTOP().getValue(); // array
			astack.pop();
			astack.push(newobj(vmobject(arrays[k2].data[k1])));
			break;
		case opcode::VADE:
			k = STACKTOP().getValue();
			astack.pop();
			arrays.erase(k);
			break;
		case opcode::LDFLD:
			k1 = STACKTOP().getValue(); // index
			astack.pop();
			k2 = STACKI(); // object
			astack.pop();
			astack.push(memory->get(k2).get(k1));
			break;
		case opcode::STFLD:
			k1 = STACKTOP().getValue(); // index
			astack.pop();
			k2 = STACKI(); // object
			astack.pop();
			memory->get(k2).set(k1, astack.getTop());
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
			k1 = memory->get(k).type;
			if (k1 == DefaultType::Int) PUSHVAL(4);
			else if (k1 == DefaultType::String) PUSHVAL(memory->get(k).bsize);
			else if (k1 == DefaultType::ArrayPointer) PUSHVAL(arrays[memory->get(k).getValue()].data.size);
			astack.pop();
			PUSHVAL(arrays[k].data.size);
			break;
		case opcode::STELEM:
			k1 = STACKTOP().getValue(); // index
			astack.pop();
			k2 = STACKTOP().getValue(); // array
			astack.pop();
			arrays[k2].data[k1] = astack.getTop();
			astack.pop();
			break;
		case opcode::PUSHL:
			PUSHVAL(Array<byte>(i.parameters, 4));
			break;
		case opcode::ADD:
			k1 = STACKI();
			astack.pop();
			k2 = STACKI();
			astack.pop();
			PUSHNEW(vmobject::binaryop(memory->get(k1), memory->get(k2), 0));
			break;
		case opcode::CMP:
			cv1 = STACKTOP().getValue();
			astack.pop();
			cv2 = STACKTOP().getValue();
			astack.pop();
			less = false;
			greater = false;
			equal = false;
			zero = false;
			if (cv1 < cv2) less = true;
			if (cv1 > cv2) greater = true;
			if (cv1 == cv2) equal = true;
			break;
		case opcode::RET:
			ret();
			break;
		case opcode::EMIT:
			k = PARAMI(0);
			v = globals->get(k);
			emc = *disassembler::disassembleCode(v.holder, v.size);
			addr = bytecode->size;
			for (n = 0; n < emc.size; n++) bytecode->push(emc[n]);
			branch(addr);
			break;
		case opcode::LEAP:
			leap(memory->get(STACKTOP().getValue()).getValue());
			astack.pop();
			break;
		case opcode::BR:
			branch(memory->get(STACKTOP().getValue()).getValue());
			astack.pop();
		case opcode::JMP:
			pc = memory->get(STACKTOP().getValue()).getValue();
			astack.pop();
			break;
		case opcode::IFEQ:
			if (!equal) pc += 1;
			break;
		case opcode::IFNE:
			if (equal) pc += 1;
			break;
		case opcode::IFLE:
			if (!equal && greater) pc += 1;
			break;
		case opcode::IFGE:
			if (!equal && less) pc += 1;
			break;
		case opcode::IFLT:
			if (greater || equal) pc += 1;
			break;
		case opcode::IFGT:
			if (less || equal) pc += 1;
			break;
		case opcode::BREAK:
			// TODO: inform debugger?
			break;
		case opcode::INTR:
			inter = i.parameters[0];
			v = Array<byte>(STACKTOP().boundValue, STACKTOP().bsize);
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
			globals->set(PARAMI(0), astack.getTop());
			astack.pop();
			break;
		case opcode::LDSTR:
			PUSHVAL(vmobject(Array<byte>(i.parameters, i.psize)));
			break;
		case opcode::LDI:
			PUSHVAL(INTE(i.parameters, 0));
			break;
		case opcode::LDB:
			PUSHVAL(Array<byte>(i.parameters, 1));
			break;
		case opcode::SPOP:
			astack.pop();
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

int nvm::newobj(vmobject& o)
{
	ii = 0;
	while (memory->find(ii))
	{
		ii++;
	}
	memory->add(ii, o);
	return ii;
}

void nvm::processEvents()
{
	if (eventQueue.size > 0)
	{
		int eix = eventQueue.size - 1;
		for (int i = 0; i < eventQueue[eix].parameters.size; i++)
			astack.push(newobj(vmobject(eventQueue[eix].parameters[i])));
		inhandler = pc;
		branch(eventHandlers[eventQueue[eix].id]);
		eventQueue.removeAt(eix);
		eventsenabled = false;
		suspended = false;
	}
	if (timerQueue.size > 0)
	{
		int eix = timerQueue.size - 1;
		for (int i = 0; i < timerQueue[eix].parameters.size; i++)
			astack.push(newobj(vmobject(timerQueue[eix].parameters[i])));
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
		for (pair<int, pair<int, int>> p : pages)
		{
			if (p.second.first <= pc && p.second.second >= pc)
			{
				curPage = p.first;
				globals = &(globalPages->get(curPage));
				locals = &(localScopes->get(curPage)[0]);
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
	for (pair<int, pair<int, int>> p : pages)
	{
		if (p.second.first <= pc && p.second.second >= pc)
		{
			curPage = p.first;
			globals = &(globalPages->get(curPage));
			locals = &(localScopes->get(curPage))[0];
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
