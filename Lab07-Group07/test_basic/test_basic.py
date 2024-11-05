#!/usr/bin/python3

import sys
import os
import re

def sort_log_lines(log_lines):
    """Sorts log lines based on context ID and maintains order within each context."""
    def extract_ctxid(line):
        match = re.match(r"ctx (\d+):", line)
        return int(match.group(1)) if match else None

    return sorted(log_lines, key=lambda line: (extract_ctxid(line), log_lines.index(line)))

def read_and_sort_log(file_path):
    """Reads log lines from a file and returns them sorted."""
    with open(file_path, 'r') as file:
        log_lines = file.readlines()
    return sort_log_lines(log_lines)

def run_test(test_name, input_file, output_file):
    sys.stdout.write("Running test " + test_name + "... ")
    os.system("../mathserver.out " + input_file + " temp.txt")
    
    sorted_log1 = read_and_sort_log(output_file)
    sorted_log2 = read_and_sort_log("temp.txt")

    if sorted_log1 != sorted_log2:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")
    
    os.system("rm -f temp.txt")

tests = [("Basic/Test 1.1: short instruction sequence", "test1.1.in", "test1.1.out"),
         ("Basic/Test 1.2: multiple batches of log lines", "test1.2.in", "test1.2.out"),
         ("Basic/Test 1.3: compute Fibonacci numbers", "test1.3.in", "test1.3.out"),
         ("Basic/Test 1.4: list prime numbers", "test1.4.in", "test1.4.out"),
         ("Basic/Test 1.5: approximate pi", "test1.5.in", "test1.5.out")]

for test in tests:
    run_test(*test)
