import os

# move into the test_feature1 directory
os.chdir("test_feature1")
# run the test_feature1.py script
os.system("python3 test_feature1.py")
# move back into the test_feature2 directory
os.chdir("../test_feature2")
# run the test_feature2.py script
os.system("python3 test_feature2.py")
# move back into the test_feature3 directory
os.chdir("../test_feature3")
# run the test_feature3.py script
os.system("python3 test_feature3.py")
# move back into the test_feature4 directory
os.chdir("../test_feature4")
# run the test_feature4.py script
os.system("python3 test_feature4.py")
# move back into the test_feature5 directory
os.chdir("../test_feature5")
# run the test_feature5.py script
os.system("python3 test_feature5.py")
