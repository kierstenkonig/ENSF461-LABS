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

tests = [("SJF/Test 2.1: all jobs arriving at the same time", "0", "SJF", "2", "test2.1.in", "test2.1.out"),
         ("SJF/Test 2.2: all jobs arriving at the same time (w/ analysis)", "1", "SJF", "2", "test2.2.in", "test2.2.out"),
         ("SJF/Test 2.3: gap between jobs", "0", "SJF", "2", "test2.3.in", "test2.3.out"),
         ("SJF/Test 2.4: gap between jobs (w/ analysis)", "1", "SJF", "2", "test2.4.in", "test2.4.out"),
         ("SJF/Test 2.5: initial waiting time", "0", "SJF", "2", "test2.5.in", "test2.5.out"),
         ("SJF/Test 2.6: initial waiting time (w/ analysis)", "1", "SJF", "2", "test2.6.in", "test2.6.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
