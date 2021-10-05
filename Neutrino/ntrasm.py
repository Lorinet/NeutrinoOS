import re
import os.path as path
import sys
import os

OP_NOP = 0x01
OP_DELFLD = 0x02
OP_AND = 0x11
OP_OR = 0x12
OP_XOR = 0x13
OP_SHL = 0x14
OP_SHR = 0x15
OP_NOT = 0x16
OP_ST = 0x20
OP_TOSTR = 0x21
OP_CLR = 0x22
OP_MOV = 0x23
OP_CONCAT = 0x24
OP_PARSEINT = 0x25
OP_SPLIT = 0x26
OP_INDEX = 0x27
OP_SIZE = 0x28
OP_APPEND = 0x29
OP_NEWOBJ = 0x2A
OP_STB = 0x2B
OP_PUSHB = 0x2C
OP_CONCATB = 0x2D
OP_APPENDB = 0x2E
OP_CLRB = 0x2F
OP_PUSHL = 0x30
OP_SCMP = 0x31
OP_ADD = 0x40
OP_SUB = 0x41
OP_MUL = 0x42
OP_DIV = 0x43
OP_INC = 0x44
OP_DEC = 0x45
OP_IMUL = 0x46
OP_IDIV = 0x47
OP_CMP = 0x50
OP_CZ = 0x51
OP_CMPS = 0x52
OP_CMPI = 0x53
OP_CMPB = 0x54
OP_CZB = 0x55
OP_CMPIB = 0x56
OP_INSERT = 0x57
OP_VAC = 0x58
OP_LDELEM = 0x59
OP_VAD = 0x5A
OP_DELELEM = 0x5B
OP_VADE = 0x5C
OP_LDFLD = 0x5D
OP_STFLD = 0x5E
OP_SWAP = 0x5F
OP_JMP = 0x60
OP_JEQ = 0x61
OP_JNE = 0x62
OP_JLE = 0x63
OP_JGE = 0x64
OP_JLT = 0x65
OP_JGT = 0x66
OP_JZ = 0x67
OP_JNZ = 0x68
OP_RET = 0x69
OP_EMIT = 0x6A
OP_MOVPC = 0x6B
OP_LJ = 0x6C
OP_SJ = 0x6D
OP_SJE = 0x6E
OP_SJNE = 0x6F
OP_SJLE = 0x70
OP_SJGE = 0x71
OP_SJL = 0x72
OP_SJG = 0x73
OP_SJZ = 0x74
OP_SJNZ = 0x75
OP_LDLEN = 0x76
OP_STELEM = 0x77
OP_EXTMOVL = 0x78
OP_LJL = 0x79
OP_LJG = 0x7A
OP_LJE = 0x7B
OP_LJNE = 0x7C
OP_LJGE = 0x7D
OP_LJLE = 0x7E
OP_JSP = 0x7F
OP_INTS = 0x80
OP_INT = 0x81
OP_BREAK = 0x82
OP_INTB = 0x83
OP_BITS = 0x84
OP_ADDS = 0x85
OP_PUSH = 0x90
OP_POP = 0x91
OP_POPA = 0x92
OP_SPUSH = 0x93
OP_TOP = 0x94
OP_SPOP = 0x95
OP_POPB = 0x96
OP_VPUSHB = 0x97
OP_LINK = 0x98
OP_LEAP = 0x99
OP_HALT = 0xB0

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

def to_bytes(i):
    return i.to_bytes(4, "little")

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

def instr_branch(opr, ops, lbl):
    global pcode;
    global labels;
    lab = lbl.replace(":", "")
    if lab in labels:
        addr = labels[lab]
        if addr < 256:
            instr_simple(ops)
            pcode.append(to_byte(addr))
        else:
            instr_simple(opr)
            pcode.extend(to_bytes(addr))
    else:
        rage_quit(11, "label not found: " + lab)

def instr_lbranch(op, lbl):
    global pcode;
    global labels;
    lab = lbl.replace(":", "")
    if lab in labels:
        addr = labels[lab]
        instr_simple(op)
        pcode.extend(to_bytes(addr))
    else:
        rage_quit(11, "label not found: " + lab)

def instr_byte_var(opr, opb, var1):
    global pcode;
    global var;
    cr_var(var1)
    vk1 = var[var1]
    if vk1 < 256:
        instr_simple(opb)
        pcode.append(to_byte(vk1))
    else:
        instr_var(opr, var1)

def instr_byte_var_var(opr, opb, var1, var2):
    global pcode;
    global var;
    cr_var(var1)
    cr_var(var2)
    vk1 = var[var1]
    vk2 = var[var2]
    if vk1 < 256 and vk2 < 256:
        instr_simple(opb)
        pcode.append(to_byte(vk1))
        pcode.append(to_byte(vk2))
    else:
        instr_var_var(opr, var1, var2)

def instr_byte_var(opr, opb, var1):
    global pcode;
    global var;
    cr_var(var1)
    vk1 = var[var1]
    if vk1 < 256:
        instr_simple(opb)
        pcode.append(to_byte(vk1))
    else:
        instr_var(opr, var1)

def instr_var_var_var(op, var1, var2, var3):
    global pcode;
    global var;
    instr_simple(op)
    cr_var(var1)
    cr_var(var2)
    cr_var(var3)
    pcode.extend(to_bytes(var[var1]))
    pcode.extend(to_bytes(var[var2]))
    pcode.extend(to_bytes(var[var3]))

def instr_var_var(op, var1, var2):
    global pcode;
    global var;
    instr_simple(op)
    cr_var(var1)
    cr_var(var2)
    pcode.extend(to_bytes(var[var1]))
    pcode.extend(to_bytes(var[var2]))

def instr_var_ival(op, var1, ival):
    global pcode;
    global var;
    instr_simple(op)
    cr_var(var1)
    pcode.extend(to_bytes(var[var1]))
    pcode.extend(to_bytes(ival))

def instr_var(op, var1):
    global pcode;
    global var;
    instr_simple(op)
    cr_var(var1)
    pcode.extend(to_bytes(var[var1]))


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
    if code[i].startswith("leap"):
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
            code[i] = s_remove(code[i], 0, 4)
            code[i] = "call" + code[i];
        else:
            code[i] = "leap " + str(oi) + " " + str(si)
    elif code[i].startswith("extmovl"):
        sym = code[i].split(' ')[1]
        lv = code[i].split(' ')[2]
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
        code[i] = "extmovl " + str(oi) + " " + str(si) + " " + str(lv)
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
                if cl.startswith("jmp") or cl.startswith("call") or cl.startswith("goto") or cl.startswith("jz") or cl.startswith("jnz") or cl.startswith("jeq") or cl.startswith("jne") or cl.startswith("jlt") or cl.startswith("jgt") or cl.startswith("jle") or cl.startswith("jge") or cl.startswith("pushl") or cl.startswith("lj") or cl.startswith("lje") or cl.startswith("ljne") or cl.startswith("ljl") or cl.startswith("ljg") or cl.startswith("ljle") or cl.startswith("ljge"):
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
    elif op == "and":
        instr_var_var(OP_AND, arg[1], arg[2])
    elif op == "or":
        instr_var_var(OP_OR, arg[1], arg[2])
    elif op == "xor":
        instr_var_var(OP_XOR, arg[1], arg[2])
    elif op == "shl":
        instr_var_var(OP_SHL, arg[1], arg[2])
    elif op == "shr":
        instr_var_var(OP_SHR, arg[1], arg[2])
    elif op == "not":
        instr_var(OP_NOT, arg[1])
    elif op == "str":
        cr_var(arg[1])
        instr_simple(OP_ST)
        if arg[2] == "\"\"":
            pcode.extend(to_bytes(4))
            pcode.extend(to_bytes(var[arg[1]]))
        elif s[5 + len(arg[1])] == '"':
            val = ""
            for i in range(6 + len(arg[1]), len(s) - 1):
                if i == len(s):
                    val = s_remove(val, len(val) - 1, 1)
                    break
                val += s[i]
            val = val.replace("\\0", "\0").replace("\\n", "\n").replace("\\\n", "\\n")
            pcode.extend(to_bytes(4 + len(val)))
            pcode.extend(to_bytes(var[arg[1]]))
            pcode.extend(s_to_bytes(val))
        else:
            value = int_lit(s, 5 + len(arg[1]))
            pcode.extend(to_bytes(8))
            pcode.extend(to_bytes(var[arg[1]]))
            pcode.extend(to_bytes(value))
    elif op == "stb":
        cr_var(arg[1])
        instr_simple(OP_STB)
        value = int_lit(s, 5 + len(arg[1]))
        pcode.extend(to_bytes(var[arg[1]]))
        pcode.append(to_byte(value))
    elif op == "pushb":
        instr_simple(OP_PUSHB)
        value = int_lit(s, 6)
        pcode.append(to_byte(value))
    elif op == "string" or op == "tostr":
        instr_simple(OP_TOSTR)
    elif op == "clr":
        cr_var(arg[1])
        vkey = var[arg[1]]
        if vkey < 256:
            instr_simple(OP_CLRB)
            pcode.append(to_byte(vkey))
        else:
            instr_simple(OP_CLR)
            pcode.extend(to_bytes(vkey))
    elif op == "mov":
        instr_var_var(OP_MOV, arg[1], arg[2])
    elif op == "pushl":
        if arg[1].replace(":", "") in labels:
            instr_simple(OP_PUSHL)
            pcode.extend(to_bytes(labels[arg[1].replace(":", "")]))
        else:
            rage_quit(9, "invalid label: " + arg[1])
    elif op == "movpc":
        instr_var(OP_MOVPC, arg[1])
    elif op == "split":
        instr_var_var(OP_SPLIT, arg[1], arg[2])
        pcode.extend(to_bytes(int_lit(arg[3], 0)))
    elif op == "concat":
        instr_byte_var_var(OP_CONCAT, OP_CONCATB, arg[1], arg[2])
    elif op == "integer":
        instr_simple(OP_PARSEINT)
    elif op == "index":
        instr_var_var_var(OP_INDEX, arg[1], arg[2], arg[3])
    elif op == "inst":
        instr_var_var_var(OP_INSERT, arg[1], arg[2], arg[3])
    elif op == "vac":
        instr_simple(OP_VAC)
    elif op == "vad":
        instr_simple(OP_VAD)
    elif op == "ldfld":
        instr_simple(OP_LDFLD)
    elif op == "stfld":
        instr_simple(OP_STFLD)
    elif op == "vade":
        instr_simple(OP_VADE)
    elif op == "delelem":
        instr_simple(OP_DELELEM)
    elif op == "ldelem":
        instr_simple(OP_LDELEM)
    elif op == "swap":
        instr_simple(OP_SWAP)
    elif op == "ldlen":
        instr_simple(OP_LDLEN)
    elif op == "stelem":
        instr_simple(OP_STELEM)
    elif op == "size":
        instr_var_var(OP_SIZE, arg[1], arg[2])
    elif op == "append":
        instr_byte_var_var(OP_APPEND, OP_APPENDB, arg[1], arg[2])
    elif op == "newobj":
        instr_simple(OP_NEWOBJ)
    elif op == "add":
        instr_simple(OP_ADD)
    elif op == "sub":
        instr_simple(OP_SUB)
    elif op == "mul":
        instr_simple(OP_MUL)
    elif op == "div":
        instr_simple(OP_DIV)
    elif op == "inc":
        instr_var_ival(OP_INC, arg[1], int_lit(s, 5 + len(arg[1])))
    elif op == "dec":
        instr_var_ival(OP_DEC, arg[1], int_lit(s, 5 + len(arg[1])))
    elif op == "imul":
        instr_var_ival(OP_IMUL, arg[1], int_lit(s, 6 + len(arg[1])))
    elif op == "idiv":
        instr_var_ival(OP_IDIV, arg[1], int_lit(s, 6 + len(arg[1])))
    elif op == "scmp":
        instr_simple(OP_SCMP)
    elif op == "cmp":
        instr_byte_var_var(OP_CMP, OP_CMPB, arg[1], arg[2])
    elif op == "cz":
        instr_byte_var(OP_CZ, OP_CZB, arg[1])
    elif op == "cmpi":
        cr_var(arg[1])
        if s[6 + len(arg[1])] == '"':
            val = ""
            for i in range(7 + len(arg[1]), len(s) - 1):
                if i == len(s.replace("\\0", "\0").replace("\\n", "\n")):
                    val = s_remove(val, len(val) - 1, 1)
                    break
                val += s.replace("\\0", "\0").replace("\\n", "\n")[i]
            instr_simple(OP_CMPS)
            pcode.extend(to_bytes(4 + len(val)))
            pcode.extend(to_bytes(var[arg[1]]))
            pcode.extend(s_to_bytes(val))
        else:
            value = int_lit(s, 6 + len(arg[1]))
            vkey = var[arg[1]]
            if vkey < 256 and value < 256:
                instr_simple(OP_CMPIB)
                pcode.append(to_byte(vkey))
                pcode.append(to_byte(value))
            else:
                instr_simple(OP_CMPI)
                pcode.extend(to_bytes(var[arg[1]]))
                pcode.extend(to_bytes(value))
    elif op == "jmp" or op == "goto" or op == "call":
        instr_branch(OP_JMP, OP_SJ, arg[1])
    elif op == "jeq":
        instr_branch(OP_JEQ, OP_SJE, arg[1])
    elif op == "jne":
        instr_branch(OP_JNE, OP_SJNE, arg[1])
    elif op == "jle":
        instr_branch(OP_JLE, OP_SJLE, arg[1])
    elif op == "jge":
        instr_branch(OP_JGE, OP_SJGE, arg[1])
    elif op == "jlt":
        instr_branch(OP_JLT, OP_SJL, arg[1])
    elif op == "jgt":
        instr_branch(OP_JGT, OP_SJG, arg[1])
    elif op == "jz":
        instr_branch(OP_JZ, OP_SJZ, arg[1])
    elif op == "jnz":
        instr_branch(OP_JNZ, OP_SJNZ, arg[1])
    elif op == "emit":
        instr_var(OP_EMIT, arg[1])
    elif op == "ret":
        instr_simple(OP_RET)
    elif op == "lj":
        instr_lbranch(OP_LJ, arg[1])
    elif op == "lje":
        instr_lbranch(OP_LJE, arg[1])
    elif op == "ljne":
        instr_lbranch(OP_LJNE, arg[1])
    elif op == "ljl":
        instr_lbranch(OP_LJL, arg[1])
    elif op == "ljg":
        instr_lbranch(OP_LJG, arg[1])
    elif op == "ljle":
        instr_lbranch(OP_LJLE, arg[1])
    elif op == "ljge":
        instr_lbranch(OP_LJGE, arg[1])
    elif op == "jsp":
        instr_simple(OP_JSP)
    elif op == "ints":
        instr_simple(OP_INTS)
        interrupt = int_lit(arg[1], 0)
        if s[6 + len(arg[1])] == '"':
            val = ""
            for i in range(7 + len(arg[1]), len(s) - 1):
                if i == len(s.replace("\\0", "\0").replace("\\n", "\n")):
                    val = s_remove(val, len(val) - 1, 1)
                    break
                val += s.replace("\\0", "\0").replace("\\n", "\n")[i]
            pcode.extend(to_bytes(1 + len(val)))
            pcode.append(to_byte(interrupt))
            pcode.extend(s_to_bytes(val))
        else:
            value = int_lit(s, 6 + len(arg[1]))
            pcode.extend(to_bytes(5))
            pcode.append(to_byte(interrupt))
            pcode.extend(to_bytes(value))
    elif op == "int" and len(arg) == 2:
        instr_simple(OP_INTS)
        pcode.extend(to_bytes(1))
        pcode.append(to_byte(int_lit(arg[1], 0)))
    elif op == "int" and len(arg) > 2:
        cr_var(arg[2])
        interrupt = int_lit(arg[1], 0)
        vark = var[arg[2]]
        if vark < 256:
            instr_simple(OP_INTB)
            pcode.append(to_byte(interrupt))
            pcode.append(to_byte(vark))
        else:
            instr_simple(OP_INT)
            pcode.append(to_byte(interrupt))
            pcode.extend(to_bytes(vark))
    elif op == "bits":
        instr_simple(OP_BITS)
        pcode.append(to_byte(int_lit(arg[1], 0)))
    elif op == "adds":
        instr_simple(OP_ADDS)
    elif op == "break":
        instr_simple(OP_BREAK)
    elif op == "push":
        instr_byte_var(OP_PUSH, OP_VPUSHB, arg[1])
    elif op == "pop":
        instr_byte_var(OP_POP, OP_POPB, arg[1])
    elif op == "spop":
        instr_simple(OP_SPOP)
    elif op == "spush" or op == "ldstr":
        instr_simple(OP_SPUSH)
        if s[6] == '"':
            val = ""
            for i in range(7, len(s) - 1):
                if i == len(s.replace("\\0", "\0").replace("\\n", "\n")):
                    val = s_remove(val, len(val) - 1, 1)
                    break
                val += s.replace("\\0", "\0").replace("\\n", "\n")[i]
            pcode.extend(to_bytes(len(val)))
            pcode.extend(s_to_bytes(val))
        else:
            value = int_lit(arg[1], 0)
            pcode.extend(to_bytes(4))
            pcode.extend(to_bytes(value))
    elif op == "top":
        instr_simple(OP_TOP)
        pcode.extend(to_bytes(int_lit(arg[1], 0)))
    elif op == "link":
        pcode.append(OP_LINK)
        pcode.append(to_byte(len(arg[1])))
        pcode.extend(s_to_bytes(arg[1]))
    elif op == "leap":
        instr_simple(OP_LEAP)
        pcode.extend(to_bytes(int_lit(arg[1], 0)))
        pcode.extend(to_bytes(int_lit(arg[2], 0)))
    elif op == "extmovl":
        instr_simple(OP_EXTMOVL)
        pcode.extend(to_bytes(int_lit(arg[1], 0)))
        pcode.extend(to_bytes(int_lit(arg[2], 0)))
        cr_var(arg[3])
        pcode.extend(to_bytes(var[arg[3]]))
    elif op == "halt" or op == "leave":
        instr_simple(OP_HALT)
    elif not op.startswith(":") and not op.startswith(";"):
        rage_quit(12, "invalid term " + op + " (instruction " + str(pc) + ", line '" + s + "')")
    pc += 1

if "-silent" not in flags:
    print("writing NEX executable [" + binary + "]...")

with open(binary, "wb") as f:
    f.write(bytearray(pcode))