from __future__ import print_function
import io
import os
import random

# Function: tenRandomChar
# Description: Fills empty string with 10 random lowercase chars and returns
#              the string.
# Precondition: random is seeded
def tenRandomChar ():
  str = ""
  for x in range(0,10):
    # append string with random int in range [97,122](lowercase range) converted to char
    str += chr(random.randint(97,122))
  return str
# Function: twoRandAndSum
# Description: Generates two random integers in range [1, 41]
#              Returns the two integers and the sum in a tuple
# Preconditions: random is seeded
def twoRandAndSum ():
  rand1 = random.randint(1,42)
  rand2 = random.randint(1,42)
  return rand1, rand2, rand1 + rand2

# seed random number generator
# default seed either 1) default seed for os, if available, or 2) current time
random.seed()
# get myprogram.py process id
pid = os.getpid()

# open file1[pid], file2[pid], and file3[pid] for writing
file1 = open("file1" + str(pid), 'w')
file2 = open("file2" + str(pid), 'w')
file3 = open("file3" + str(pid), 'w')

# generate 3 strings of 10 random chars ending in newline
str1 = tenRandomChar() + "\n"
str2 = tenRandomChar() + "\n"
str3 = tenRandomChar() + "\n"

# write each string to a file
file1.write(str1)
file2.write(str2)
file3.write(str3)

# print the file contents to console
print(str1 + str2 + str3, end="")
# print two random numbers and their sum to the console
print("{0[0]}\n{0[1]}\n{0[2]}".format(twoRandAndSum()))
