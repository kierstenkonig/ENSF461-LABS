#!/usr/bin/python3

import sys
import os

def run_test(test_name, input_file, output_file):
    sys.stdout.write("Running test " + test_name + "... ")
    os.system("../engine.out " + input_file + "> temp.txt")
    if os.system("diff temp.txt " + output_file + " > /dev/null") != 0:
        print("\033[91mFAILED\033[0m")
        sys.exit(1)
    else:
        print("\033[92mPASSED\033[0m")

tests = [("Test 1.1: one command", "test1.1.in", "test1.1.out"),
         ("Test 1.2: two commands", "test1.2.in", "test1.2.out"),
         ("Test 1.3: quotation marks", "test1.3.in", "test1.3.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
