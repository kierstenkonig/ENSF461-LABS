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

tests = [("RR/Test 4.1: three back-to-back jobs, slice 2", "0", "RR", "2", "test4.1.in", "test4.1.out"),
         ("RR/Test 4.2: three back-to-back jobs, slice 2 (w/ analysis)", "1", "RR", "2", "test4.2.in", "test4.2.out"),
         ("RR/Test 4.3: gap between jobs, slice 2", "0", "RR", "2", "test4.3.in", "test4.3.out"),
         ("RR/Test 4.4: gap between jobs, slice 2 (w/ analysis)", "1", "RR", "2", "test4.4.in", "test4.4.out"),
         ("RR/Test 4.5: regular pattern, slice 3", "0", "RR", "3", "test4.5.in", "test4.5.out"),
         ("RR/Test 4.6: regular pattern, slice 3 (w/ analysis)", "1", "RR", "3", "test4.6.in", "test4.6.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
