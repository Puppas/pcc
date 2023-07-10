import subprocess
import glob
import os

pcc = "../build/pcc"
test_files = glob.glob("*.c")
assembly_files = [test_file.replace(".c", ".s") for test_file in test_files]
exe_files = [test_file.replace(".c", ".exe") for test_file in test_files]


def clear():
    for assembly_file in assembly_files:
        os.remove(assembly_file)
    for exe_file in exe_files:
        os.remove(exe_file)


for test_file, assembly_file, exe_file in zip(test_files, assembly_files, exe_files):
    subprocess.run(f"gcc -o- -E -P -C {test_file} | {pcc} -o {assembly_file} -", shell=True)
    subprocess.run(f"gcc -o {exe_file} {assembly_file} -xc common", shell=True)


for exe_file in exe_files:
    res = subprocess.run(f"echo {exe_file}; ./{exe_file}", shell=True)
    if res.returncode != 0:
        clear()
        exit(1)

subprocess.run("./driver.sh", shell=True)
clear()
