import os
import os.path
import sys
import distutils.dir_util
import shutil

def copytree(src, dst):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            distutils.dir_util.copy_tree(s, d, False, None)
        else:
            shutil.copy2(s, d)

rootfs = ""
if len(sys.argv) >= 2:
    rootfs = sys.argv[1]
else:
    print("Please specify rootfs directory")
    exit(-1)

print("Installing files...")

root = os.path.splitdrive(sys.executable)[0]
if root == "":
    root = "/"
else:
    root += "\\"
if not os.path.exists(os.path.join(root, "neutrino")):
    os.system("mkdir " + os.path.join(root, "neutrino"))

copytree(rootfs, os.path.join(root, "neutrino"))