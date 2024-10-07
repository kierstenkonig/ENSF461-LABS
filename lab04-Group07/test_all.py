import os

os.chdir("test_fifo")
os.system("python3 test_fifo.py")
os.chdir("../test_sjf")
os.system("python3 test_sjf.py")
os.chdir("../test_stcf")
os.system("python3 test_stcf.py")
os.chdir("../test_rr")
os.system("python3 test_rr.py")
os.chdir("../test_lt")
os.system("python3 test_lt.py")
