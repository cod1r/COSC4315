import subprocess
import os
input_files = sorted(os.listdir("./testcases"))[:-2]
for input_file in input_files:
    subprocess.run(f"./minipython testcases/{input_file}", shell=True)
