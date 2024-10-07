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

tests = [("LT/Test 5.1: two jobs w/ equal duration, slice 25", "0", "LT", "25", "test5.1.in", "test5.1.out"),
         ("LT/Test 5.2: two jobs w/ equal duration, slice 25 (w/ analysis)", "1", "LT", "25", "test5.2.in", "test5.2.out"),
         ("LT/Test 5.3: two jobs w/ different duration, slice 25", "0", "LT", "25", "test5.3.in", "test5.3.out"),
         ("LT/Test 5.4: two jobs w/ different duration, slice 25 (w/ analysis)", "1", "LT", "25", "test5.4.in", "test5.4.out"),
         ("LT/Test 5.5: gaps between jobs, slice 5", "0", "LT", "5", "test5.5.in", "test5.5.out"),
         ("LT/Test 5.6: gaps between jobs, slice 5 (w/ analysis)", "1", "LT", "5", "test5.6.in", "test5.6.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
