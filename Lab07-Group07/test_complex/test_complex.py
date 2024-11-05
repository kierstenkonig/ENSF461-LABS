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

tests = [("Complex/Test 3.1: more Fibonacci", "test3.1.in", "test3.1.out"),
         ("Complex/Test 3.2: multiple complex operations", "test3.2.in", "test3.2.out"),
         ("Complex/Test 3.3: simple+complex operations", "test3.3.in", "test3.3.out"),
         ("Complex/Test 3.4: longer operation mix", "test3.4.in", "test3.4.out"),
         ("Complex/Test 3.5: CPU buster", "test3.5.in", "test3.5.out")]

for test in tests:
    run_test(*test)
