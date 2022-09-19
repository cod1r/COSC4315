#!/usr/bin/env python3
import subprocess
for i in range(1, 11):
    subprocess.run(f"./freqnumber.py \"k=3;input=tc{i}.txt;output=tc{i}_out.out\"", shell=True)
for i in range(1, 11):
    subprocess.run(f"diff -q tc{i}.out tc{i}_out.out", shell=True)
