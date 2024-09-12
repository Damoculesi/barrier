import os
from subprocess import check_output
import re
import matplotlib.pyplot as plt

# Define the range for -l parameter (number of loops)
L_VALUES = [10, 50, 100, 500, 600, 700, 800, 900, 1000]

# Define threads to use (0 means sequential)
THREADS = [0, 2, 4, 8, 16, 32]

INPUT_FILE = "tests/8k.txt"  # Adjust to your input file
OUTPUT_FILE = "temp_output.txt"

# Function to get the execution time for a given thread count and -l value
def get_execution_time(n_threads, l_value):
    cmd = f"./bin/prefix_scan -n {n_threads} -i {INPUT_FILE} -o {OUTPUT_FILE} -l {l_value}"
    try:
        out = check_output(cmd, shell=True).decode("ascii")
        m = re.search("time: (\d+)", out)
        if m:
            return int(m.group(1))
    except Exception as e:
        print(f"Error running command: {cmd}")
        print(e)
    return None

# Store times for plotting
times_sequential = []
times_parallel = []

# Run for each value of -l (loop parameter)
for l_value in L_VALUES:
    # Sequential run
    time_seq = get_execution_time(0, l_value)
    times_sequential.append(time_seq)
    
    # Parallel run (using the maximum number of threads)
    time_parallel = get_execution_time(8, l_value)  # Adjust this to the number of threads you're testing
    times_parallel.append(time_parallel)
    
    print(f"-l {l_value}: Sequential = {time_seq} μs, Parallel = {time_parallel} μs")

# Find the inflexion point (where the sequential and parallel times are closest)
differences = [abs(s - p) for s, p in zip(times_sequential, times_parallel)]
inflexion_index = differences.index(min(differences))
inflexion_l_value = L_VALUES[inflexion_index]

print(f"Inflexion point found at -l = {inflexion_l_value}, where Sequential time = {times_sequential[inflexion_index]} μs and Parallel time = {times_parallel[inflexion_index]} μs")

# Plotting the results
plt.plot(L_VALUES, times_sequential, label="Sequential")
plt.plot(L_VALUES, times_parallel, label="Parallel (8 threads)", linestyle="--")
plt.axvline(x=inflexion_l_value, color='red', linestyle=':', label=f'Inflexion at -l {inflexion_l_value}')
plt.xlabel('-l (Loop Parameter)')
plt.ylabel('Execution Time (μs)')
plt.title('Sequential vs Parallel Execution Times')
plt.legend()
plt.grid(True)
plt.show()
