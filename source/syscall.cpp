#include "syscall.h"
#include "nvm.h"
#include "vt.h"
#include "event.h"
#include "lvmgr.h"
#include "vmmgr.h"
#include "input.h"
#include "sysinfo.h"
#include "ntime.h"
#include "timer.h"
#include "components.h"
#include "kernlog.h"
#include "filesystem.h"
string syscall::filename, syscall::files, syscall::folders, syscall::txt, syscall::txt1, syscall::txt2;
Array<byte> syscall::contents, syscall::msg;
unsigned int syscall::n;
int syscall::proc, syscall::id, syscall::id2;
byte syscall::c;
bool syscall::bl;
vt syscall::vi, syscall::vo;
interrupts syscall::syscll;
IntMap<iapi*> syscall::interfaces;
Array<byte> syscall::data;
mutex syscall::syscallMutex;
Array<byte> syscall::systemCall(byte* indata, int datasize, nvm* v)
{
	lock_guard<mutex> lock(syscallMutex);
	syscll = (interrupts)indata[0];
	data.clear();
	eventid eid;
	for (unsigned int i = 1; i < datasize; i++) data.push(indata[i]);
#if defined(__SDL)
	SDL_Color sc;
#endif
	switch (syscll)
	{
	case interrupts::PRINT:
		v->outterm->write(data);
		break;
	case interrupts::NEWLN:
		v->outterm->write("\n");
		break;
	case interrupts::GETNOMSG:
		return bitconverter::toArray(v->messages.size);
		break;
	case interrupts::FGETS:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::fileExists(filename))
		{
			return file::readAllBytes(filename);
		}
		else
		{
			vmmgr::vmmerror("File not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::FPUTS:
		filename = "";
		n = 0;
		while (data[n] != 0)
		{
			filename += (char)data[n];
			n += 1;
		}
		n += 4;
		contents = Array<byte>(data.size - filename.size() - 4);
		while (n < data.size)
		{
			contents[n - filename.size() - 4] = data[n];
			n += 1;
		}
		filename = lvmgr::formatPath(filename);
		if(file::fileExists(filename))
		{
			file::writeAllBytes(filename, contents);
		}
		else
		{
			vmmgr::vmmerror("File not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::FDELETE:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::fileExists(filename))
		{
			file::deleteFile(filename);
		}
		else
		{
			vmmgr::vmmerror("File not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::FCREATE:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(!file::fileExists(filename))
		{
			file::createFile(filename);
		}
		break;
	case interrupts::DDELETE:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::directoryExists(filename))
		{
			file::deleteDirectory(filename);
		}
		else
		{
			vmmgr::vmmerror("Folder not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::DCREATE:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(!file::directoryExists(filename))
		{
			file::createDirectory(filename);
		}
		break;
	case interrupts::CGETS:
		v->awaitin = true;
	case interrupts::CSCAN:
		break;
	case interrupts::CGETK:
		break;
	case interrupts::START_PROCESS:
		proc = bitconverter::toint32(data, 0);
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			vmmgr::getProcess(proc)->suspended = false;
		}
		break;
	case interrupts::GETFILES:
		id = 0;
		while (v->arrays.find(id) != v->arrays.end())
		{
			id++;
		}
		v->arrays.insert({ id, arrayobj() });
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::directoryExists(filename))
		{
			vector<string> fls = file::getFiles(filename);
			for (string s : fls)
				v->arrays[id].data.push(bitconverter::toArray(s));
			return bitconverter::toArray(id);
		}
		else
		{
			vmmgr::vmmerror("Folder not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::GETDIRECTORIES:
		id = 0;
		while (v->arrays.find(id) != v->arrays.end())
		{
			id++;
		}
		v->arrays.insert({ id, arrayobj() });
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::directoryExists(filename))
		{
			for (string s : file::getDirectories(filename))
				v->arrays[id].data.push(bitconverter::toArray(s));
			return bitconverter::toArray(id);
		}
		else
		{
			vmmgr::vmmerror("Folder not found: " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::FILE_EXISTS:
		return bitconverter::toArray(file::fileExists(lvmgr::formatPath(bitconverter::tostring(data))));
	case interrupts::FOLDER_EXISTS:
		return bitconverter::toArray(file::directoryExists(lvmgr::formatPath(bitconverter::tostring(data))));
	case interrupts::POWER:
		switch (data[0])
		{
			case 0x00:
				#ifdef __ESP32
				esp_restart();
				#endif
				break;
			case 0x01:
#if defined(COMPONENT_EFFIGY)
				WindowManager::Stop();
#endif
#if defined(__UNIX)
				vmmgr::shutdown();
#endif
				exit(0);
				break;
			case 0x02:
				break;
			case 0x03:
			break;
		}
		break;
	case interrupts::GETFIRSTMSG:
		if (v->messages.size > 0)
		{
			msg = v->messages[0];
			v->messages.removeAt(0);
			return msg;
		}
		break;
	case interrupts::CREATE_PROCESS:
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::fileExists(filename))
		{
        	return bitconverter::toArray(vmmgr::createProcess(filename));
		}
		else
		{
			vmmgr::vmmerror("Can't find file " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::CREATE_BACKGROUND_PROCESS:
		break;
	case interrupts::KILL_PROCESS:
		proc = bitconverter::toint32(data, 0);
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			vmmgr::terminateProcess(proc);
		}
		break;
	case interrupts::GET_PROCESSID:
		return bitconverter::toArray(v->processid);
	case interrupts::SHUTDOWN_PROCESS:
		v->running = false;
		break;
	case interrupts::FORK_PROCESS:
		vmmgr::createProcess(v->fileName);
		break;
	case interrupts::RESTORE_SCREEN:

		break;
	case interrupts::CPRINTEDIT:

		break;
	case interrupts::CREATE_PROCESS_EX:
		c = data[0];
		data.removeAt(0);
		switch (c)
		{
		case procexih::STDIN_MSGOUT:
			vi = vt(0, v->processid, vtype::StandardInput);
			vo = vt(0, v->processid, vtype::MessageOut);
			break;
		case procexih::MSGIN_MSGOUT:
			vi = vt(0, v->processid, vtype::MessageIn);
			vo = vt(0, v->processid, vtype::MessageOut);
			break;
		case procexih::STDIN_STDOUT:
			vi = vt(0, v->processid, vtype::StandardInput);
			vo = vt(0, v->processid, vtype::StandardOutput);
			break;
		case procexih::MSGIN_STDOUT:
			vi = vt(0, v->processid, vtype::MessageIn);
			vo = vt(0, v->processid, vtype::StandardOutput);
			break;
		}
		filename = lvmgr::formatPath(bitconverter::tostring(data));
		if(file::fileExists(filename))
		{
			return bitconverter::toArray(vmmgr::createProcessEx(filename, vi, vo));
		}
		else
		{
			vmmgr::vmmerror("Can't find file " + bitconverter::tostring(data), v->processid);
		}
		break;
	case interrupts::SEND_PROC_INPUT:
		proc = bitconverter::toint32(data, 0);
		msg = Array<byte>();
		for (n = 4; n < data.size; n++) msg.push(data[n]);
		msg.push('\n');
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			vmmgr::sendInput(proc, msg);
		}
		break;
	case interrupts::GETMSG:
		if (v->messages.size > 0)
		{
			msg = v->messages.getTop();
			v->messages.pop();
			return msg;
		}
		break;
	case interrupts::SENDMSG:
		proc = bitconverter::toint32(data, 0);
		msg = Array<byte>();
		for (n = 4; n < data.size; n++) msg.push(data[n]);
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			vmmgr::sendMessage(proc, msg);
		}
		break;
	case interrupts::AWAITMSG:
		v->awaitmsg = true;
		break;
	case interrupts::THROWERR:
		vmmgr::vmmerror(bitconverter::tostring(data), v->processid);
		break;
	case interrupts::WAIT_FOR_PROC_REQUEST_INPUT:
		proc = bitconverter::toint32(data, 0);
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			v->waitForProcInput = proc;
		}
		break;
	case interrupts::SUSPEND_PROCESS_EX:
		proc = bitconverter::toint32(data, 0);
		if(vmmgr::getProcess(proc) == NULL)
		{
			vmmgr::vmmerror("Can't find PID " + proc, v->processid);
		}
		else
		{
			vmmgr::getProcess(proc)->suspended = true;
		}
		break;
	case interrupts::GETINFOSTRING:
		switch (data[0])
		{
		case 0x00:
			return bitconverter::toArray(neutrinoOSVersion);
		case 0x01:
			return bitconverter::toArray(aboutNeutrino);
		case 0x02:
			return bitconverter::toArray(neutrinoDeviceType);
		case 0x03:
			return bitconverter::toArray(neutrinoOSVersion);
		case 0x04:
			return bitconverter::toArray(neutrinoBuildDate);
		case 0x05:
			return bitconverter::toArray(neutrinoBuildNumber);
		case 0x06:
			return bitconverter::toArray(platformArch);
		}
		break;
	case interrupts::ATTACH_EVENT_HANDLER:
		eid = (eventid)data[0];
		proc = bitconverter::toint32(data, 1);
		v->eventHandlers.insert({ eid, proc });
		break;
	case interrupts::DETACH_EVENT_HANDLER:
		eid = (eventid)data[0];
		if (v->eventHandlers.count(eid)) v->eventHandlers.erase(eid);
		break;
	case interrupts::ENABLE_EVENTS:
		v->eventsenabled = true;
		break;
	case interrupts::DISABLE_EVENTS:
		v->eventsenabled = false;
		break;
	case interrupts::SUSPEND:
		v->suspended = true;
		break;
	case interrupts::WAKE:
		v->suspended = false;
		break;
	case interrupts::GETTIME:
		switch (data[0])
		{
		case timesel::TIME_HM:
			return bitconverter::toArray(ntime::getTimeS());
			break;
		case timesel::TIME_HMS:
			return bitconverter::toArray(ntime::getTimeWithSeconds());
			break;
		case timesel::DATETIME:
			return bitconverter::toArray(ntime::getDateTime());
			break;
		case timesel::HOUR:
			return bitconverter::toArray(ntime::getHour());
			break;
		case timesel::MINUTE:
			return bitconverter::toArray(ntime::getMinute());
			break;
		case timesel::SECOND:
			return bitconverter::toArray(ntime::getSecond());
			break;
		case timesel::YEAR:
			return bitconverter::toArray(ntime::getYear());
			break;
		case timesel::MONTH:
			return bitconverter::toArray(ntime::getMonth());
			break;
		case timesel::DAY:
			return bitconverter::toArray(ntime::getDay());
			break;
		}
		break;
	case interrupts::SETTIMEZONE:
		ntime::setTimeZone(bitconverter::toint32(data, 0));
		if (ntime::getDST()) ntime::timeZone += 1;
		break;
	case interrupts::TIMERINT:
		switch (data[0])
		{
		case timercmd::CREATE_TIMER:
			id = 0;
			while (v->timers.find(id) != v->timers.end()) id += 1;
			v->timers.insert({ id, timerevt(bitconverter::toint32(data, 1), bitconverter::toint32(data, 5)) });
			return bitconverter::toArray(id);
			break;
		case timercmd::DESTROY_TIMER:
			id = bitconverter::toint32(data, 1);
			if (v->timers.find(id) != v->timers.end()) v->timers.erase(v->timers.find(id));
			break;
		case timercmd::LOOP:
			id = bitconverter::toint32(data, 1);
			if (v->timers.find(id) != v->timers.end()) v->timers[id].loop = true;
			break;
		case timercmd::ONCE:
			id = bitconverter::toint32(data, 1);
			if (v->timers.find(id) != v->timers.end()) v->timers[id].loop = false;
			break;
		case timercmd::START_TIMER:
			id = bitconverter::toint32(data, 1);
			if (v->timers.find(id) != v->timers.end())
			{
				v->timers[id].startTime = ntime::getMillis();
				v->timers[id].running = true;
			}
			break;
		case timercmd::STOP_TIMER:
			id = bitconverter::toint32(data, 1);
			if (v->timers.find(id) != v->timers.end()) v->timers[id].running = false;
			break;
		}
		break;
	case interrupts::COMPONENT_CALL:
		msg = Array<byte>(data);
		msg.removeAt(0);
		contents = interfaces[data[0]]->message(msg, v);
		if (contents.size > 0) return Array<byte>(contents);
		break;
	default:
		break;
	}
	return Array<byte>();
}
