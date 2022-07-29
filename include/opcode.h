#pragma once
enum opcode
{
	OP_NOP = 0x01,
	OP_DELFLD = 0x02, // delete field
	OP_LDLOC = 0x03,  // store local
	OP_STLOC = 0x04,  // load local
	OP_SWSCOP = 0x05,
	OP_LDLOCB = 0x06,
	OP_STLOCB = 0x07,
	OP_SWSCOPB = 0x08,
	OP_AND = 0x11,
	OP_OR = 0x12,
	OP_XOR = 0x13,
	OP_SHL = 0x14,
	OP_SHR = 0x15,
	OP_NOT = 0x16,
	OP_PWR = 0x17,
	OP_MOD = 0x18,
	OP_TOSTR = 0x21,
	OP_CLR = 0x22,
	OP_PARSEINT = 0x25,
	OP_INDEX = 0x27,
	OP_NEWOBJ = 0x2A, // new object
	OP_LDGLB = 0x2C,
	OP_PUSHL = 0x30,
	OP_CAST = 0x31,
	OP_ADD = 0x40,
	OP_SUB = 0x41,
	OP_MUL = 0x42,
	OP_DIV = 0x43,
	OP_CMP = 0x50,
	OP_VAC = 0x58,   // array create
	OP_LDELEM = 0x59,// array index get
	OP_VAD = 0x5A,   // array add
	OP_DELELEM = 0x5B,   // array remove
	OP_VADE = 0x5C,  // array deallocate
	OP_LDFLD = 0x5D, // load field
	OP_STFLD = 0x5E, // store field
	OP_SWAP = 0x5F,
	OP_LEAP = 0x60,
	OP_IFEQ = 0x61,
	OP_IFNE = 0x62,
	OP_IFLE = 0x63,
	OP_IFGE = 0x64,
	OP_IFLT = 0x65,
	OP_IFGT = 0x66,
	OP_RET = 0x69,
	OP_EMIT = 0x6A,
	OP_BR = 0x6B,
	OP_JMP = 0x6C,
	OP_BRE = 0x6D,
	OP_IRET = 0x6E,
	OP_LDLEN = 0x76,  // array length
	OP_STELEM = 0x77, // array set index
	OP_PUSHLX = 0x78,
	OP_GC = 0x79,
	OP_INTR = 0x81,
	OP_BREAK = 0x82,
	OP_LDGL = 0x90,
	OP_STGL = 0x91,
	OP_LDI = 0x92,
	OP_LDSTR = 0x93,
  	OP_TOP = 0x94,
	OP_SPOP = 0x95,
	OP_STGLB = 0x96,
	OP_DUP = 0x97,
	OP_LINK = 0x98,
	OP_LDB = 0x99,
	OP_LDIB = 0x9A,
	OP_TOPB = 0x9B,
	OP_LDSTRB = 0x9C
};