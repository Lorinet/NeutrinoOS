#pragma once
enum interrupts
{
	PRINT = 0x10,
	NEWLN = 0x12,
	BCOLOR = 0x13,
	FCOLOR = 0x14,
	FGETS = 0x15,
	FPUTS = 0x16,
	FDELETE = 0x17,
	FCREATE = 0x18,
	DCREATE = 0x19,
	DDELETE = 0x1A,
	CGETS = 0x1B,
	CSCAN = 0x1C,
	CGETK = 0x1D,
	START_PROCESS = 0x1E,
	GETFILES = 0x1F,
	GETDIRECTORIES = 0x20,
	FILE_EXISTS = 0x21,
	FOLDER_EXISTS = 0x22,
	POWER = 0x23,
	SETTITLE = 0x24,
	CREATE_PROCESS = 0x25,
	KILL_PROCESS = 0x26,
	GET_PROCESSID = 0x27,
	CREATE_BACKGROUND_PROCESS = 0x28,
	CLEAR_SCREEN = 0x29,
	BUFFER_SCREEN = 0x2A,
	RESTORE_SCREEN = 0x2B,
	CPRINTEDIT = 0x2C,
	CREATE_PROCESS_EX = 0x2D,
	SEND_PROC_INPUT = 0x2E,
	GETMSG = 0x2F,
	SENDMSG = 0x30,
	AWAITMSG = 0x31,
	THROWERR = 0x32,
	WAIT_FOR_PROC_REQUEST_INPUT = 0x33,
	SUSPEND_PROCESS_EX = 0x34,
	GETINFOSTRING = 0x35,
	GRAPHICSINT = 0x36,
	ATTACH_EVENT_HANDLER = 0x37,
	DETACH_EVENT_HANDLER = 0x38,
	ENABLE_EVENTS = 0x39,
	DISABLE_EVENTS = 0x3A,
	SUSPEND = 0x3B,
	WAKE = 0x3C,
	GETTIME = 0x3D,
	SETTIMEZONE = 0x3E,
	TIMERINT = 0x3F,
	PIN_INTERRUPT = 0x40,
	SERIAL_INTERRUPT = 0x41,
	NFS_INTERRUPT = 0x42,
	UI_INTERRUPT = 0x47
};
enum nfscmd
{
	OPEN = 0x00,
	CLOSE = 0x01,
	CREATE = 0x02,
	ERASE = 0x03,
	ADDFILE = 0x04,
	REMOVEFILE = 0x05,
	WRITEFILE = 0x06,
	READFILE = 0x07,
	EXTRACTIMAGE = 0x08,
	COPYFILE = 0x09,
	MOVEFILE = 0x0A
};
enum procexih
{
	STDIN_MSGOUT = 0, // new process gets input from inherited console, and transmits output to parent through IPC message
	MSGIN_STDOUT = 1, // new process gets input from IPC message and writes output to the inherited console
	MSGIN_MSGOUT = 2, // new process gets input and sends output through IPC messaging
	STDIN_STDOUT = 3  // new process inherits console from parent
};
enum graphcmd
{
	CLEARSCR = 0x00,
	SETCOLOR = 0x01,
	DRAWPOINT = 0x02,
	DRAWRECT = 0x03,
	FILLRECT = 0x04,
	DRAWLINE = 0x05,
	SETFONT = 0x06,
	DRAWSTRING = 0x07,
	SETPOS = 0x08,
	INIT = 0x09,
	DRAWXBITMAP = 0x0A,
	SETXBMDIMS = 0x0B
};
enum timesel
{
	TIME_HM = 0x00,
	TIME_HMS = 0x01,
	DATETIME = 0x02,
	HOUR = 0x03,
	MINUTE = 0x04,
	SECOND = 0x05,
	YEAR = 0x06,
	MONTH = 0x07,
	DAY = 0x08
};
enum timercmd
{
	CREATE = 0x00,
	DESTROY = 0x01,
	LOOP = 0x02,
	ONCE = 0x03,
	START = 0x04,
	STOP = 0x05
};
enum pinint
{
	PININIT = 0x00,
	DIGITALWRITE = 0x01,
	DIGITALREAD = 0x02,
	PWMWRITE = 0x03
};
enum serint
{
	SERINIT = 0x00,
	SERWRITE = 0x01,
	SERREAD = 0x02,
	SERREADBYTES = 0x03,
	SERREADBYTE = 0x04,
	SERAVAILABLE = 0x05,
	SERCLOSE = 0x06
};