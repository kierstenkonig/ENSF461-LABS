import os

os.chdir("test_basic")
os.system("python3 test_basic.py")
os.chdir("../test_parallel")
os.system("python3 test_parallel.py")
os.chdir("../test_complex")
os.system("python3 test_complex.py")
