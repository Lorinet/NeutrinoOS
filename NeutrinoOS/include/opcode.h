#pragma once
enum opcode
{
	NOP = 0x01,
	DELFLD = 0x02, // delete field
	AND = 0x11,
	OR = 0x12,
	XOR = 0x13,
	SHL = 0x14,
	SHR = 0x15,
	NOT = 0x16,
	ST = 0x20,
	TOSTR = 0x21,
	CLR = 0x22,
	MOV = 0x23,
	CONCAT = 0x24,
	PARSE = 0x25,
	SPLIT = 0x26,
	INDEX = 0x27,
	SIZ = 0x28,
	APPEND = 0x29,
	NEWOBJ = 0x2A, // new object
	STB = 0x2B,
	PUSHB = 0x2C,
	CONCATB = 0x2D,
	APPENDB = 0x2E,
	CLRB = 0x2F,
	PUSHL = 0x30,
	SCMP = 0x31,
	ADD = 0x40,
	SUB = 0x41,
	MUL = 0x42,
	DIV = 0x43,
	INC = 0x44,
	DEC = 0x45,
	IMUL = 0x46,
	IDIV = 0x47,
	CMP = 0x50,
	CZ = 0x51,
	CMPS = 0x52,
	CMPI = 0x53,
	CMPB = 0x54,
	CZB = 0x55,
	CMPIB = 0x56,
	INSERT = 0x57,
	VAC = 0x58,   // array create
	LDELEM = 0x59,// array index get
	VAD = 0x5A,   // array add
	DELELEM = 0x5B,   // array remove
	VADE = 0x5C,  // array deallocate
	LDFLD = 0x5D, // load field
	STFLD = 0x5E, // store field
	SWAP = 0x5F,
	JMP = 0x60,
	JEQ = 0x61,
	JNE = 0x62,
	JLE = 0x63,
	JGE = 0x64,
	JLT = 0x65,
	JGT = 0x66,
	JZ = 0x67,
	JNZ = 0x68,
	RET = 0x69,
	EMIT = 0x6A,
	MOVPC = 0x6B,
	LJ = 0x6C,
	SJ = 0x6D,
	SJE = 0x6E,
	SJNE = 0x6F,
	SJLE = 0x70,
	SJGE = 0x71,
	SJL = 0x72,
	SJG = 0x73,
	SJZ = 0x74,
	SJNZ = 0x75,
	LDLEN = 0x76, // array length
	STELEM = 0x77, // array set index
    EXTMOVL = 0x78,
	LJL = 0x79,
	LJG = 0x7A,
	LJE = 0x7B,
	LJNE = 0x7C,
	LJGE = 0x7D,
	LJLE = 0x7E,
	JSP = 0x7F,
	INTS = 0x80,
	INTR = 0x81,
	BREAK = 0x82,
	INTB = 0x83,
	BITS = 0x84,
	PUSH = 0x90,
	POP = 0x91,
	POPA = 0x92,
	SPUSH = 0x93,
	TOP = 0x94,
	SPOP = 0x95,
	POPB = 0x96,
	VPUSHB = 0x97,
	LINK = 0x98,
	EXTCALL = 0x99,
	HALT = 0xB0
};