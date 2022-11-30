import subprocess
import os
input_files = sorted(os.listdir("./testcases/testcases_phase1"))
input_files2 = sorted(os.listdir("./testcases/testcases_phase2"))
for input_file in input_files:
    subprocess.run(f"./minipython testcases/testcases_phase1/{input_file}", shell=True)
for input_file in input_files2:
    subprocess.run(f"./minipython testcases/testcases_phase2/{input_file}", shell=True)
