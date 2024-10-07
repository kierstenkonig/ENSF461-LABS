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

tests = [("STCF/Test 3.1: all jobs arriving at the same time", "0", "STCF", "2", "test3.1.in", "test3.1.out"),
         ("STCF/Test 3.2: all jobs arriving at the same time (w/ analysis)", "1", "STCF", "2", "test3.2.in", "test3.2.out"),
         ("STCF/Test 3.3: interrupted job", "0", "STCF", "2", "test3.3.in", "test3.3.out"),
         ("STCF/Test 3.4: interrupted job (w/ analysis)", "1", "STCF", "2", "test3.4.in", "test3.4.out"),
         ("STCF/Test 3.5: multiple interruptions", "0", "STCF", "2", "test3.5.in", "test3.5.out"),
         ("STCF/Test 3.6: multiple interruptions (w/ analysis)", "1", "STCF", "2", "test3.6.in", "test3.6.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
