import re
import os.path as path
import sys
import os

OP_NOP = 0x01
OP_DELFLD = 0x02
OP_LDLOC = 0x03
OP_STLOC = 0x04
OP_SWSCOP = 0x05
OP_LDLOCB = 0x06
OP_STLOCB = 0x07
OP_SWSCOPB = 0x08
OP_AND = 0x11
OP_OR = 0x12
OP_XOR = 0x13
OP_SHL = 0x14
OP_SHR = 0x15
OP_NOT = 0x16
OP_PWR = 0x17
OP_MOD = 0x18
OP_TOSTR = 0x21
OP_CLR = 0x22
OP_PARSEINT = 0x25
OP_INDEX = 0x27
OP_NEWOBJ = 0x2A
OP_LDGLB = 0x2C
OP_PUSHL = 0x30
OP_SCMP = 0x31
OP_ADD = 0x40
OP_SUB = 0x41
OP_MUL = 0x42
OP_DIV = 0x43
OP_CMP = 0x50
OP_VAC = 0x58
OP_LDELEM = 0x59
OP_VAD = 0x5A
OP_DELELEM = 0x5B
OP_VADE = 0x5C
OP_LDFLD = 0x5D
OP_STFLD = 0x5E
OP_SWAP = 0x5F
OP_LEAP = 0x60
OP_IFEQ = 0x61
OP_IFNE = 0x62
OP_IFLE = 0x63
OP_IFGE = 0x64
OP_IFLT = 0x65
OP_IFGT = 0x66
OP_RET = 0x69
OP_EMIT = 0x6A
OP_BR = 0x6B
OP_JMP = 0x6C
OP_BRP = 0x6D
OP_LDLEN = 0x76
OP_STELEM = 0x77
OP_PUSHLX = 0x78
OP_GC = 0x79
OP_INTR = 0x81
OP_BREAK = 0x82
OP_LDGL = 0x90
OP_STGL = 0x91
OP_LDI = 0x92
OP_LDSTR = 0x93
OP_TOP = 0x94
OP_SPOP = 0x95
OP_STGLB = 0x96
OP_DUP = 0x97
OP_LINK = 0x98
OP_LDB = 0x99
OP_LDIB = 0x9A
OP_TOPB = 0x9B
OP_LDSTRB = 0x9C

source = ""
binary = ""
base_dir = ""
include_paths = []
lib_paths = []
includes = []
pc = 0
vi = 0
var = {}
labels = {}
defines = {}
code = []
pcode = []
flags = []

def include(fil):
    if fil not in includes:
        includes.append(fil)
    c = []
    if path.exists(os.path.join(base_dir, fil)):
        with open(os.path.join(base_dir, fil)) as f:
            c = f.readlines()
    else:
        done = False
        for include_path in include_paths:
            if path.exists(os.path.join(include_path, fil)):
                with open(os.path.join(include_path, fil)) as f:
                    c = f.readlines()
                    done = True
                    break
        if not done:
            rage_quit(1, "cannot open included file: " + fil)
    for i in range(len(c)):
        if c[i][len(c[i]) - 1] == '\n':
            c[i] = s_remove(c[i], len(c[i]) - 1, 1);
    for line in c:
        if line.startswith("#include") and line.split(' ')[1] not in includes:
            includes.append(line.split(' ')[1])
            include(line.split(' ')[1])
    return

def s_replace(line, fro, to):
    if line == "":
        return ""
    return re.sub("\\b" + re.escape(fro) + "\\b", to, line)

def s_remove(line, fro, to):
    return line[0:fro] + line[fro + to:]

def to_bytes(intv):
    return intv.to_bytes(4, "little")

def s_to_bytes(s):
    return bytearray(s, "cp1252")

def to_byte(i):
    return i.to_bytes(1, "little")[0]

def int_lit(str, off):
    value = 0
    try:
        value = int(s_remove(str, 0, off), 10)
    except:
        value = int(s_remove(str, 0, off + 2), 16)
    return value

def rage_quit(ec, err):
    print("error: " + err)
    exit(ec)

def cr_var(v):
    global var;
    global vi;
    if v not in var:
        var[v] = vi
        vi += 1

def instr_simple(op):
    global pcode;
    pcode.append(op)

def instr_byte_ival(opr, opb, ival):
    global pcode;
    if ival < 256:
        instr_simple(opb)
        pcode.append(to_byte(ival))
    else:
        instr_simple(opr)
        pcode.extend(to_bytes(ival))

def instr_byte_var(opr, opb, name):
    global pcode;
    global var;
    cr_var(name)
    vk = var[name]
    if vk < 256:
        instr_simple(opb)
        pcode.append(to_byte(vk))
    else:
        instr_simple(opr)
        pcode.extend(to_bytes(vk))

if len(sys.argv) == 2:
    if sys.argv[1] == "-help":
        print("usage:\npython ntrasm.py <inputFile> [outputFile] [-options]\noptions:\n-genRelocTable: include symbol table in NEX header (for dynamic linking support)\n-genModuleFile: create linkable module descriptor file alongside main executable\n-silent: silent mode, does not write to stdout\n-verbose: show compilation line-by-line\n-includeDirectory=<dir>: add include directory\n-libraryDirectory=<dir>: add library directory\nfor more info visit https://lorinet.github.io/neutrino/docs/ntrasm")
        exit(-1)
    else:
        source = sys.argv[1]
        binary = source.replace(".ns", ".lex")
elif len(sys.argv) == 3:
    if sys.argv[2].startswith("-"):
        source = sys.argv[1]
        binary = source.replace(".ns", ".lex")
        flags.append(sys.argv[2])
    else:
        source = sys.argv[1]
        binary = sys.argv[2]
elif len(sys.argv) > 3:
    if sys.argv[2].startswith("-"):
        source = sys.argv[1]
        binary = source.replace(".ns", ".lex")
        flags.append(sys.argv[2])
    else:
        source = sys.argv[1]
        binary = sys.argv[2]
    for i in range(3, len(sys.argv)):
        flags.append(sys.argv[i])
else:
    print("python ntrasm.py -help for usage information")
    exit(-1)

for f in flags:
    if f.startswith("-includeDirectory="):
        include_paths.append(f.split('=')[1])
    elif f.startswith("-libraryDirectory="):
        lib_paths.append(f.split('=')[1])

if len(include_paths) == 0:
    root = os.path.splitdrive(sys.executable)[0]
    if root == "":
        root = "/"
    else:
        root += "\\"
    include_paths = [os.path.join(root, "neutrino", "ndk", "include")]

if len(lib_paths) == 0:
    root = os.path.splitdrive(sys.executable)[0]
    if root == "":
        root = "/"
    else:
        root += "\\"
    lib_paths = [os.path.join(root, "neutrino", "ndk", "lib")]

if path.exists(source):
    base_dir = os.path.dirname(source)
    with open(source) as f:
        code = f.readlines()
else:
    rage_quit(-2, "cannot open source file: " + source)

if "-silent" not in flags:
    print("Neutrino IL Assembler")
    print("assembling [" + source + "]...")

for i in range(len(code)):
    if code[i][len(code[i]) - 1] == '\n':
        code[i] = s_remove(code[i], len(code[i]) - 1, 1);

includes.append(source)
for s in code:
    if s.startswith("#include") and s.split(' ')[1] not in includes:
        include(s.split(' ')[1])
includes.remove(source)

for s in includes:
    included_code = []
    if path.exists(os.path.join(base_dir, s)):
        with open(os.path.join(base_dir, s)) as f:
            included_code = f.readlines()
    else:
        done = False
        for include_path in include_paths:
            if path.exists(os.path.join(include_path, s)):
                with open(os.path.join(include_path, s)) as f:
                    included_code = f.readlines()
                    done = True
                    break
        if not done:
            rage_quit(1, "cannot open included file: " + s)
    for i in range(len(included_code)):
        if included_code[i][len(included_code[i]) - 1] == '\n':
            included_code[i] = s_remove(included_code[i], len(included_code[i]) - 1, 1);
    code.extend(included_code)

i = 0
while i < len(code):
    if code[i].startswith("#include"):
        del code[i]
        i -= 1
    i += 1

for s in code:
    if s.startswith("#define"):
        name = s.split(' ')[1]
        cnt = s_remove(s, 0, 9 + len(name))
        if name not in defines:
            defines[name] = cnt

i = 0
while i < len(code):
    if code[i].startswith("#define"):
        del code[i]
        i -= 1
    i += 1

for i in range(len(code)):
    for defn in defines:
        code[i] = s_replace(code[i], defn, defines[defn])

while "" in code:
    code.remove("")

pcode.append(s_to_bytes('N')[0])
pcode.append(s_to_bytes('E')[0])
pcode.append(s_to_bytes('X')[0])
pc = 0
extmtds = {}
lnximports = []
obi = 0
for s in code:
    if s.startswith("link"):
        mdf = s.split(' ')[1].replace(".lnx", ".lmd")
        mdl = []
        if path.exists(mdf):
            with open(mdf) as f:
                mdl = f.readlines()
        else:
            done = False
            for lib_path in lib_paths:
                if path.exists(os.path.join(lib_path, mdf)):
                    with open(os.path.join(lib_path, mdf)) as f:
                        mdl = f.readlines()
                        done = True
                        break
            if not done:
                rage_quit(2, "cannot open linkable module descriptor for library " + mdf)
        lnximports.append(s.split(' ')[1])
        mtds = {}
        for m in mdl:
            mtds[m.split(':')[0]] = int(m.split(':')[1])
        extmtds[(s.split(' ')[1], obi)] = mtds
        obi += 1

for i in range(len(code)):
    if code[i].startswith("pushlx"):
        sym = code[i].split(' ')[1]
        si = 0
        oi = 0
        found = False
        for v in extmtds:
            if sym in extmtds[v]:
                si = extmtds[v][sym]
                oi = v[1]
                found = True
        if not found:
            rage_quit(3, "symbol not found: " + sym)
        code[i] = "pushlx " + str(oi) + " " + str(si)
sections = {}
sec = True
for i in range(len(code)):
    lt = ""
    if code[i].startswith(":"):
        lt = s_remove(code[i], 0, 1)
        sec = True
        ts = []
        while sec:
            i += 1
            if code[i].startswith(":"):
                i -= 1
                break
            if code[i] != "" and not code[i].startswith(";"):
                ts.append(code[i])
            if code[i].startswith("ret"):
                sec = False
        if lt not in sections:
            sections[lt] = ts
        else:
            rage_quit(4, "label " + lt + " is already defined")
executedSections = []
linkedSections = []
if "-genRelocTable" in flags:
    for kvp in sections:
        executedSections.append(kvp)
else:
    executedSections.append("main")
    for kvp in sections:
        for cl in sections[kvp]:
            spl = cl.split(' ')
            if len(spl) > 1:
                lbl = spl[1].replace(":", "")
                if spl[0] == "pushl" or spl[0] == "brp":
                    if lbl not in executedSections:
                        executedSections.append(lbl)
executedCode = []
pc = 0
for s in executedSections:
    labels[s] = pc
    try:
        for cl in sections[s]:
            executedCode.append(cl)
            pc += 1
    except:
        rage_quit(5, "could not find label " + s)

if "-cleanCode" in flags:
    with open("cc.ns", "w") as f:
        f.writelines(executedCode)

if "-genRelocTable" in flags:
    if "-silent" not in flags:
        print("creating symbol table...")
    for s in code:
        if s.startswith("#exlink"):
            linkedSections.append(s.split(' ')[1])
    pcode.append(s_to_bytes('L')[0])
    pcode.extend(to_bytes(len(linkedSections)))
    lksi = 0
    for s in linkedSections:
        pcode.extend(to_bytes(lksi))
        pcode.extend(to_bytes(labels[s]))
        lksi += 1
    lksi = 0
    if "-genModuleFile" in flags:
        modl = []
        for s in linkedSections:
            modl.append(s + ":" + str(lksi) + "\n")
            lksi += 1
        if "-silent" not in flags:
            print("writing module descriptor file [" + s_remove(binary, len(binary) - 4, 4) + ".lmd]")
        with open(s_remove(binary, len(binary) - 4, 4) + ".lmd", "w") as f:
            f.writelines(modl)
else:
    pcode.append(s_to_bytes('E')[0])
pc = 0
for s in executedCode:
    if "-verbose" in flags:
        print(s)
    arg = s.split(' ')
    op = arg[0].lower()
    if op == "nop":
        instr_simple(OP_NOP)
    elif op == "delfld":
        instr_simple(OP_DELFLD)
    elif op == "ldloc":
        instr_byte_var(OP_LDLOC, OP_LDLOCB, arg[1])
    elif op == "stloc":
        instr_byte_var(OP_STLOC, OP_STLOCB, arg[1])
    elif op == "swscop":
        instr_byte_ival(OP_SWSCOP, OP_SWSCOPB, int_lit(arg[1], 0))
    elif op == "and":
        instr_simple(OP_AND)
    elif op == "or":
        instr_simple(OP_OR)
    elif op == "xor":
        instr_simple(OP_XOR)
    elif op == "shl":
        instr_simple(OP_SHL)
    elif op == "shr":
        instr_simple(OP_SHR)
    elif op == "not":
        instr_simple(OP_NOT)
    elif op == "pwr":
        instr_simple(OP_PWR)
    elif op == "mod":
        instr_simple(OP_MOD)
    elif op == "string" or op == "tostr":
        instr_simple(OP_TOSTR)
    elif op == "clr":
        instr_simple(OP_CLR)
    elif op == "parseint":
        instr_simple(OP_PARSEINT)
    elif op == "index":
        instr_simple(OP_INDEX)
    elif op == "newobj":
        instr_simple(OP_NEWOBJ)
    elif op == "ldgl":
        instr_byte_var(OP_LDGL, OP_LDGLB, arg[1])
    elif op == "stgl":
        instr_byte_var(OP_STGL, OP_STGLB, arg[1])
    elif op == "pushl":
        if arg[1].replace(":", "") in labels:
            instr_simple(OP_PUSHL)
            pcode.extend(to_bytes(labels[arg[1].replace(":", "")]))
        else:
            rage_quit(9, "invalid label: " + arg[1])
    elif op == "add":
        instr_simple(OP_ADD)
    elif op == "sub":
        instr_simple(OP_SUB)
    elif op == "mul":
        instr_simple(OP_MUL)
    elif op == "div":
        instr_simple(OP_DIV)
    elif op == "cmp":
        instr_simple(OP_CMP)
    elif op == "vac":
        instr_simple(OP_VAC)
    elif op == "ldelem":
        instr_simple(OP_LDELEM)
    elif op == "vad":
        instr_simple(OP_VAD)
    elif op == "delelem":
        instr_simple(OP_DELELEM)
    elif op == "vade":
        instr_simple(OP_VADE)
    elif op == "ldfld":
        instr_simple(OP_LDFLD)
    elif op == "stfld":
        instr_simple(OP_STFLD)
    elif op == "swap":
        instr_simple(OP_SWAP)
    elif op == "leap":
        instr_simple(OP_LEAP)
    elif op == "ifeq":
        instr_simple(OP_IFEQ)
    elif op == "ifne":
        instr_simple(OP_IFNE)
    elif op == "ifle":
        instr_simple(OP_IFLE)
    elif op == "ifge":
        instr_simple(OP_IFGE)
    elif op == "iflt":
        instr_simple(OP_IFLT)
    elif op == "ifgt":
        instr_simple(OP_IFGT)
    elif op == "ret":
        instr_simple(OP_RET)
    elif op == "gc":
        instr_simple(OP_GC)
    elif op == "dup":
        instr_simple(OP_DUP)
    elif op == "emit":
        instr_simple(OP_EMIT)
    elif op == "br":
        instr_simple(OP_BR)
    elif op == "brp":
        if arg[1].replace(":", "") in labels:
            instr_simple(OP_BRP)
            pcode.extend(to_bytes(labels[arg[1].replace(":", "")]))
        else:
            rage_quit(9, "invalid label: " + arg[1])
    elif op == "jmp":
        instr_simple(OP_JMP)
    elif op == "ldlen":
        instr_simple(OP_LDLEN)
    elif op == "stelem":
        instr_simple(OP_STELEM)
    elif op == "pushlx":
        instr_simple(OP_PUSHLX)
        pcode.extend(to_bytes(int_lit(arg[1], 0)))
        pcode.extend(to_bytes(int_lit(arg[2], 0)))
    elif op == "intr":
        instr_simple(OP_INTR)
        pcode.append(int_lit(arg[1], 0))
    elif op == "break":
        instr_simple(OP_BREAK)
    elif op == "ldi":
        instr_byte_ival(OP_LDI, OP_LDIB, int_lit(arg[1], 0));
    elif op == "ldstr":
        val = ""
        for i in range(7, len(s) - 1):
            if i == len(s.replace("\\0", "\0").replace("\\n", "\n")):
                val = s_remove(val, len(val) - 1, 1)
                break
            if i > 0:
                if s[i] == '"' and s[i - 1] != '\\':
                    break
            val += s.replace("\\0", "\0").replace("\\n", "\n")[i]
        if len(val) > 255:
            instr_simple(OP_LDSTR)
            pcode.extend(to_bytes(len(val)))
        else:
            instr_simple(OP_LDSTRB)
            pcode.append(to_byte(len(val)))
        pcode.extend(s_to_bytes(val))
    elif op == "spop":
        instr_simple(OP_SPOP)
    elif op == "ldb":
        instr_simple(OP_LDB)
        pcode.append(int_lit(arg[1], 0))
    elif op == "link":
        instr_simple(OP_LINK)
        pcode.append(to_byte(len(arg[1])))
        pcode.extend(s_to_bytes(arg[1]))
    elif op == "top":
        instr_byte_ival(OP_TOP, OP_TOPB, int_lit(arg[1], 0))
    elif not op.startswith(":") and not op.startswith(";"):
        rage_quit(12, "invalid term " + op + " (instruction " + str(pc) + ", line '" + s + "')")
    pc += 1

if "-silent" not in flags:
    print("writing NEX executable [" + binary + "]...")

with open(binary, "wb") as f:
    f.write(bytearray(pcode))
