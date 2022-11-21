import subprocess
import os
input_files = sorted(os.listdir("./testcases/testcases_phase1"))
for input_file in input_files:
    subprocess.run(f"./minipython testcases/testcases_phase1/{input_file}", shell=True)
