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

tests = [("Test 4.1: basic pipe", "test4.1.in", "test4.1.out"),
         ("Test 4.2: pipe + redirection", "test4.2.in", "test4.2.out"),
         ("Test 4.3: more pipe + redirection", "test4.3.in", "test4.3.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
