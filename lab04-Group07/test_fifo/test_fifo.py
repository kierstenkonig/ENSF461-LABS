#!/usr/bin/python3

import sys
import os

def run_test(test_name, analysis, policy, slice, input_file, output_file):
    sys.stdout.write("Running test " + test_name + "... ")
    os.system("../scheduler.out " + analysis + " " + policy + " " + slice + " " + input_file + " > temp.txt")
    if os.system("diff temp.txt " + output_file + " > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")

tests = [("FIFO/Test 1.1: all jobs arriving at the same time", "0", "FIFO", "2", "test1.1.in", "test1.1.out"),
         ("FIFO/Test 1.2: all jobs arriving at the same time (w/ analysis)", "1", "FIFO", "2", "test1.2.in", "test1.2.out"),
         ("FIFO/Test 1.3: gap between jobs", "0", "FIFO", "2", "test1.3.in", "test1.3.out"),
         ("FIFO/Test 1.4: gap between jobs (w/ analysis)", "1", "FIFO", "2", "test1.4.in", "test1.4.out"),
         ("FIFO/Test 1.5: initial waiting time", "0", "FIFO", "2", "test1.5.in", "test1.5.out"),
         ("FIFO/Test 1.6: initial waiting time (w/ analysis)", "1", "FIFO", "2", "test1.6.in", "test1.6.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
