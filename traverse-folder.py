import os
import sys

usage_str = "usage: " + os.path.basename(__file__) + " folder"
if(len(sys.argv) != 2):
    print(usage_str)
    sys.exit(0)

folder_pth = sys.argv[1]

folder_tups = tuple(os.walk(folder_pth))
for base, _, fs in folder_tups:
    for f in fs:
        ls = (base + "/" + f).replace("\\", "/")
        print(ls)

print("--------------------")
for base, _, fs in folder_tups:
    if not fs: continue
    ls = base + "="
    for f in fs:
        ls = ls + f + ":"
    if(ls.endswith(":")): ls = ls[0:len(ls) - 1]
    ls = ls.replace("\\", "/")
    print(ls)
