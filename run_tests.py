#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep

#  Configuration parameters
# THREADS = [0]
THREADS = [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32]  # Different numbers of threads to test
LOOPS = [10, 100, 500, 1000, 10000]  # Different loop counts
INPUTS = ["16k.txt"]  # Input files

# Flag to enable spin barrier (-s)
USE_SPIN_BARRIER = True

csvs = []
for inp in INPUTS:
    for loop in LOOPS:
        csv = ["{}/{}".format(inp, loop)]
        for thr in THREADS:
            # Construct the command based on the USE_SPIN_BARRIER flag
            cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
                thr, inp, loop)

            # Append the spin barrier flag if enabled
            if USE_SPIN_BARRIER:
                cmd += " -s"

            try:
                # Execute the command and capture output
                out = check_output(cmd, shell=True).decode("ascii")
                
                # Extract the time using regex
                m = re.search("time: (.*)", out)
                if m is not None:
                    time = m.group(1)
                    csv.append(time)

            except Exception as e:
                # Handle errors (e.g., if the command fails or segmentation fault occurs)
                print(f"Error running command: {cmd}")
                print(f"Exception: {e}")
                csv.append("ERROR")

        # Store the result for this input/loop combination
        csvs.append(csv)
        sleep(0.5)

# Print the header
header = ["microseconds"] + [str(x) for x in THREADS]
print("\n")
print(", ".join(header))

# Print the results
for csv in csvs:
    print(", ".join(csv))
