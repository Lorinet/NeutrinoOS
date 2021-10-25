import os
import os.path
import sys
import shutil

def copytree(src, dst):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, False, None)
        else:
            shutil.copy2(s, d)

# Specify order of compilation
libs = [
    "neutrino.ns",
    "util.ns",
    "io.ns",
    "timer.ns",
    "userlib.ns",
    "whiprtl.ns"
]

print("NeutrinoOS Userspace Build Script")
baseDir = os.path.dirname(os.path.realpath(__file__))
outDir = os.path.join(baseDir, "buildroot")
toolsDir = "/neutrino/Neutrino/ndk"
install = False

if len(sys.argv) >= 2:
    outDir = sys.argv[1]
if len(sys.argv) >= 3:
    if sys.argv[2] == "install":
        install = True
try:
    print("Output directory: " + outDir)
    print("Wiping output directory...")
    if os.path.exists(outDir):
        shutil.rmtree(outDir)
    os.system("mkdir " + outDir)
    outDir = os.path.join(outDir, "Neutrino")
    os.system("mkdir " + outDir)
    print("Building system libraries...")
    os.system("mkdir " + os.path.join(outDir, "sys"))
    os.system("mkdir " + os.path.join(outDir, "ndk"))
    os.system("mkdir " + os.path.join(outDir, "ndk", "lib"))
    for f in libs:
        print(f)
        os.system("python3 " + os.path.join(toolsDir, "ntrasm.py") + " " + os.path.join(baseDir, "sys", f) + " " + os.path.join(outDir, "sys", f).replace(".ns", ".lnx") + " -silent -genRelocTable -genModuleFile -includeDirectory=" + os.path.join(baseDir, "include") + " -libraryDirectory=" + os.path.join(outDir, "ndk", "lib"))
        shutil.move(os.path.join(outDir, "sys", f).replace(".ns", ".lmd"), os.path.join(outDir, "ndk", "lib", f.replace(".ns", ".lmd")))
    print("Building application binaries...")
    os.system("mkdir " + os.path.join(outDir, "bin"))
    for f in os.listdir(os.path.join(baseDir, "bin")):
        print(f)
        os.system("python3 " + os.path.join(toolsDir, "ntrasm.py") + " " + os.path.join(baseDir, "bin", f, f + ".ns") + " " + os.path.join(outDir, "bin", f) + ".lex -silent -includeDirectory=" + os.path.join(baseDir, "include") + " -libraryDirectory=" + os.path.join(outDir, "ndk", "lib"))
    print("Building system configuration...")
    os.system("mkdir " + os.path.join(outDir, "cfg"))
    copytree(os.path.join(baseDir, "cfg"), os.path.join(outDir, "cfg"))
    print("Copying resources folder...")
    os.system("mkdir " + os.path.join(outDir, "res"))
    copytree(os.path.join(baseDir, "res"), os.path.join(outDir, "res"))
    print("Copying include folder...")
    os.system("mkdir " + os.path.join(outDir, "ndk", "include"))
    copytree(os.path.join(baseDir, "include"), os.path.join(outDir, "ndk", "include"))
    if install:
        os.system("python install.py " + outDir)
    print("== BUILD FINISHED ==")
except:
    print("== BUILD FAILED ==")
