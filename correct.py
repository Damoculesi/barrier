#!/usr/bin/env python3
import os
from subprocess import check_output, run
import re
from time import sleep
import filecmp

THREADS = [0, 1, 2, 3, 4, 5, 8, 17, 25]
LOOPS = [100000]
INPUTS = ["8k.txt"]

csvs = []

for inp in INPUTS:
    for loop in LOOPS:
        csv = ["{}/{}".format(inp, loop)]

        # Run with thread count 0 and store the output in a reference file
        ref_output = "output_thread_0.txt"
        cmd = "./bin/prefix_scan -o {} -n 0 -i tests/{} -l {}".format(ref_output, inp, loop)
        check_output(cmd, shell=True)

        for thr in THREADS:
            # Run the prefix_scan for other thread counts
            output_file = "temp_{}.txt".format(thr)
            cmd = "./bin/prefix_scan -o {} -n {} -i tests/{} -l {}".format(
                output_file, thr, inp, loop)
            out = check_output(cmd, shell=True).decode("ascii")

            # Extract time from output
            m = re.search("time: (.*)", out)
            if m is not None:
                time = m.group(1)
                csv.append(time)

            # Compare the output with the reference file (thread count 0)
            if thr != 0:
                if filecmp.cmp(ref_output, output_file, shallow=False):
                    print(f"Output for thread count {thr} matches with thread count 0")
                else:
                    print(f"Output for thread count {thr} does NOT match with thread count 0")

        csvs.append(csv)
        sleep(0.5)

header = ["microseconds"] + [str(x) for x in THREADS]

print("\n")
print(", ".join(header))
for csv in csvs:
    print(", ".join(csv))
