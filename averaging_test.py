#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep

# Configuration parameters
THREADS = [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32]  # Different numbers of threads to test
LOOPS = [10, 100, 500, 1000, 2000, 10000, 100000]  # Different loop counts
INPUTS = ["1k.txt"]  # Input files

# Spin barrier configurations
SPIN_BARRIER_FLAGS = [False, True]  # Test both with and without spin barrier

# Number of runs for averaging
NUM_RUNS = 6

csvs = []
for inp in INPUTS:
    for loop in LOOPS:
        # Create two CSV entries: one for normal, one for spin barrier
        csv_normal = ["{}/{} (No Spin Barrier)".format(inp, loop)]
        csv_spin = ["{}/{} (With Spin Barrier)".format(inp, loop)]

        for thr in THREADS:
            for use_spin in SPIN_BARRIER_FLAGS:
                # Collect times for averaging
                times = []
                
                for _ in range(NUM_RUNS):
                    # Construct the command based on the current USE_SPIN_BARRIER flag
                    cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
                        thr, inp, loop)

                    # Append the spin barrier flag if enabled
                    if use_spin:
                        cmd += " -s"

                    try:
                        # Execute the command and capture output
                        out = check_output(cmd, shell=True).decode("ascii")
                        
                        # Extract the time using regex
                        m = re.search("time: (.*)", out)
                        if m is not None:
                            time = float(m.group(1))
                            times.append(time)
                    except Exception as e:
                        # Handle errors (e.g., if the command fails or segmentation fault occurs)
                        print(f"Error running command: {cmd}")
                        print(f"Exception: {e}")
                        times.append(float('nan'))  # Store NaN to indicate an error

                    sleep(0.1)  # Brief sleep between runs

                # Calculate average time
                avg_time = sum(times) / len(times) if times else float('nan')
                
                if use_spin:
                    csv_spin.append(f"{avg_time:.2f}" if not any(t != t for t in times) else "ERROR")  # Handle NaN as error
                else:
                    csv_normal.append(f"{avg_time:.2f}" if not any(t != t for t in times) else "ERROR")  # Handle NaN as error

        # Store the result for this input/loop combination
        csvs.append(csv_normal)
        csvs.append(csv_spin)

# Print the header
header = ["microseconds"] + [str(x) for x in THREADS]
print("\n")
print(", ".join(header))

# Print the results
for csv in csvs:
    print(", ".join(csv))
