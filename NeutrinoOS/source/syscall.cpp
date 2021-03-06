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
#include "iomgr.h"
#include "nfsdimp.h"
string syscall::filename, syscall::files, syscall::folders, syscall::txt, syscall::txt1, syscall::txt2;
Array<byte> syscall::contents, syscall::msg;
unsigned int syscall::n;
int syscall::proc, syscall::id, syscall::id2;
byte syscall::c;
bool syscall::bl;
vt syscall::vi, syscall::vo;
interrupts syscall::syscll;
Array<byte> syscall::data;
Array<byte> syscall::systemCall(byte* indata, int datasize, nvm* v)
{
	vmmgr::enterCriticalSection();
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
			for (string s : file::getFiles(filename))
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
				system("shutdown now")
#endif
				exit(0);
				break;
			case 0x02:
				vmmgr::reload();
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
			return bitconverter::toArray(neutrinoRuntimeVersion);
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
	case interrupts::PIN_INTERRUPT:
#ifdef FEATURE_GPIO
		switch (data[0])
		{
		case pinint::PININIT:
			IOManager::PinMode(bitconverter::toint32(data, 1), bitconverter::toint32(data, 5), bitconverter::toint32(data, 9));
			break;
		case pinint::DIGITALREAD:
			return bitconverter::toArray((byte)IOManager::PinRead(bitconverter::toint32(data, 1)));
		case pinint::DIGITALWRITE:
			IOManager::PinWrite(bitconverter::toint32(data, 1), bitconverter::toint32(data, 5));
			break;
		case pinint::PWMWRITE:
			IOManager::PWMWrite(bitconverter::toint32(data, 1), bitconverter::toint32(data, 5));
			break;
		}
#endif
		break;
	case interrupts::SERIAL_INTERRUPT:
#ifdef FEATURE_SERIAL
		switch (data[0])
		{
		case serint::SERINIT:
			IOManager::UARTBegin(bitconverter::tostring(data, 5), bitconverter::toint32(data, 1));
			break;
		case serint::SERAVAILABLE:
			return bitconverter::toarray(IOManager::UARTGetAvailableBytes(bitconverter::tostring(data, 1)));
		case serint::SERCLOSE:
			IOManager::UARTClose(bitconverter::tostring(data, 1));
			break;
		case serint::SERREADBYTE:
			return vector<byte>({ IOManager::UARTReadByte(bitconverter::tostring(data, 1)), 0, 0, 0 });
		case serint::SERREADBYTES:
			return IOManager::UARTReadBytes(bitconverter::tostring(data, 5), bitconverter::toint32(data, 1));
		case serint::SERREAD:
			return IOManager::UARTRead(bitconverter::tostring(data, 1));
		case serint::SERWRITE:
			txt = bitconverter::readto0(data, 1);
			contents = bitconverter::subvector(data, 2 + txt.size());
			IOManager::UARTWrite(txt, contents);
			break;
		}
#endif
		break;
	case interrupts::UI_INTERRUPT:
		switch (data[0])
		{
#if defined(COMPONENT_TIWAZ)
		case uicmd::CreateView:
			id = ViewManager::CreateView(UISerialization::DeserializeView(bitconverter::tostring(data, 1)));
			ViewManager::views[id]->parentProcess = v->processid;
			return bitconverter::toArray(id);
			break;
		case uicmd::DestroyView:
			ViewManager::CloseView(bitconverter::toint32(data, 1));
			break;
		case uicmd::AddElement:
			id = bitconverter::toint32(data, 1);
			txt = bitconverter::tostring(data, 5);
			ViewManager::views[id]->elements.push(UISerialization::DeserializeElement(txt));
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::ModifyElement:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			*ViewManager::views[id]->GetElementByID(id2) = UISerialization::DeserializeElement(txt);
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::DeleteElement:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			ViewManager::views[id]->elements.removeAt(ViewManager::views[id]->GetElementIndexByID(id2));
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::GetPropertyValue:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			return bitconverter::toArray(ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].GetProperty(txt));
			break;
		case uicmd::SetPropertyValue:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			bl = false;
			txt1 = "";
			txt2 = "";
			for (int i = 0; i < txt.size(); i++)
			{
				if (txt[i] == ':' && txt[i - 1] != '\\')
				{
					bl = true;
				}
				else if (!bl) txt1 += txt[i];
				else if (bl) txt2 += txt[i];
			}
			ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].properties[txt1] = util::replaceAll(txt2, "\\:", ":");
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::SwitchView:
			id = bitconverter::toint32(data, 1);
			ViewManager::activeView = id;
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::UpdateScreen:
			ViewManager::RenderView(ViewManager::views[ViewManager::activeView]);
			break;
		case uicmd::AttachEventHandler:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			proc = bitconverter::toint32(data, 9);
			ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].eventHandler = proc;
			break;
		case uicmd::DetachEventHandler:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			ViewManager::views[id]->elements[ViewManager::views[id]->GetElementIndexByID(id2)].eventHandler = -1;
			break;
#elif defined(COMPONENT_EFFIGY)
		case uicmd::CreateView:
			id = WindowManager::CreateWindowE(Serialization::DeserializeView(bitconverter::tostring(data, 1)));
			WindowManager::windows[id].parentProcess = v->processid;
			WindowManager::RenderWindows();
			return bitconverter::toArray(id);
			break;
		case uicmd::DestroyView:
			WindowManager::CloseWindow(bitconverter::toint32(data, 1));
			WindowManager::RenderWindows();
			break;
		case uicmd::AddElement:
			id = bitconverter::toint32(data, 1);
			txt = bitconverter::tostring(data, 5);
			WindowManager::windows[id].elements.push_back(Serialization::DeserializeElement(txt));
			WindowManager::RenderWindows();
			break;
		case uicmd::ModifyElement:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			*WindowManager::windows[id].GetElementByID(id2) = Serialization::DeserializeElement(txt);
			WindowManager::RenderWindows();
			break;
		case uicmd::DeleteElement:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			WindowManager::windows[id].elements.erase(WindowManager::windows[id].elements.begin() + WindowManager::windows[id].GetElementIndexByID(id2));
			WindowManager::RenderWindows();
			break;
		case uicmd::GetPropertyValue:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			return bitconverter::toArray(WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].GetProperty(txt));
			break;
		case uicmd::SetPropertyValue:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			txt = bitconverter::tostring(data, 9);
			bl = false;
			txt1 = "";
			txt2 = "";
			for (int i = 0; i < txt.size(); i++)
			{
				if (txt[i] == ':' && txt[i - 1] != '\\')
				{
					bl = true;
				}
				else if (!bl) txt1 += txt[i];
				else if (bl) txt2 += txt[i];
			}
			WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].properties[txt1] = util::replaceAll(txt2, "\\:", ":");
			WindowManager::RenderWindows();
			break;
		case uicmd::SwitchView:
			id = bitconverter::toint32(data, 1);
			WindowManager::activeWindow = id;
			WindowManager::RenderWindows();
			break;
		case uicmd::UpdateScreen:
			WindowManager::RenderWindows();
			break;
		case uicmd::AttachEventHandler:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			proc = bitconverter::toint32(data, 9);
			WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].eventHandler = proc;
			break;
		case uicmd::DetachEventHandler:
			id = bitconverter::toint32(data, 1);
			id2 = bitconverter::toint32(data, 5);
			WindowManager::windows[id].elements[WindowManager::windows[id].GetElementIndexByID(id2)].eventHandler = -1;
			break;
#else
		case uicmd::GetPropertyValue:
			return bitconverter::toArray("0");
			break;
#endif
		}
		break;
	case NFS_INTERRUPT:
		switch (data[0])
		{
		case nfscmd::OPEN:
			return bitconverter::toArray(nfsmgr::OpenImage(bitconverter::tostring(data, 1), v->processid));
		case nfscmd::CLOSE:
			nfsmgr::CloseImage(bitconverter::toint32(data, 1), v->processid);
			break;
		case nfscmd::CREATE_NFS:
			return bitconverter::toArray(nfsmgr::OpenImage(nfs(bitconverter::tostring(data, 1)), v->processid));
		case nfscmd::ERASE:
			nfsmgr::GetNFS(bitconverter::toint32(data, 1), v->processid)->Erase();
			break;
		case nfscmd::ADDFILE:
			nfsmgr::GetNFS(bitconverter::toint32(data, 1), v->processid)->AddFile(bitconverter::tostring(data, 5));
			break;
		case nfscmd::REMOVEFILE:
			nfsmgr::GetNFS(bitconverter::toint32(data, 1), v->processid)->RemoveFile(bitconverter::tostring(data, 5));
			break;
		case nfscmd::WRITEFILE:
			id = bitconverter::toint32(data, 1);
			txt = bitconverter::readto0(data, 5);
			txt1 = bitconverter::tostring(data, 5 + txt.size());
			nfsmgr::GetNFS(id, v->processid)->WriteFile(txt, bitconverter::toarray(txt1));
			break;
		case nfscmd::READFILE:
			return nfsmgr::GetNFS(bitconverter::toint32(data, 1), v->processid)->ReadFile(bitconverter::tostring(data, 5));
		case nfscmd::COPYFILE:
			id = bitconverter::toint32(data, 1);
			txt = bitconverter::readto0(data, 5);
			txt1 = bitconverter::tostring(data, 5 + txt.size());
			nfsmgr::GetNFS(id, v->processid)->CopyFile(txt, txt1);
			break;
		case nfscmd::MOVEFILE:
			id = bitconverter::toint32(data, 1);
			txt = bitconverter::readto0(data, 5);
			txt1 = bitconverter::tostring(data, 5 + txt.size());
			nfsmgr::GetNFS(id, v->processid)->MoveFile(txt, txt1);
			break;
		case nfscmd::EXTRACTIMAGE:
			nfsmgr::GetNFS(bitconverter::toint32(data, 1), v->processid)->ExtractAllFiles(bitconverter::tostring(data, 5));
			break;
		}
		break;
	default:
		break;
	}
	vmmgr::leaveCriticalSection();
	return Array<byte>();
}