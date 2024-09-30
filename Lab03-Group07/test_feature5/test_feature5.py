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

tests = [("Test 5.1: variable assignment/expansion", "test5.1.in", "test5.1.out"),
         ("Test 5.2: multiple variables", "test5.2.in", "test5.2.out"),
         ("Test 5.3: arithmetic expressions with variables", "test5.3.in", "test5.3.out"),
         ("Test 5.4: variables, pipes and redirections", "test5.4.in", "test5.4.out")]

for test in tests:
    run_test(*test)
os.system("rm -f temp.txt")
