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
def twoRandAndProduct():
  rand1 = random.randint(1,42)
  rand2 = random.randint(1,42)
  return rand1, rand2, rand1 * rand2

# seed random number generator
# default seed either 1) default seed for os, if available, or 2) current time
random.seed()
# get myprogram.py process id
pid = os.getpid()

# generate 3 strings of 10 random chars ending in newline
str1 = tenRandomChar() + "\n"
str2 = tenRandomChar() + "\n"
str3 = tenRandomChar() + "\n"

# write each string to a file
with open("file1_" + str(pid), 'w') as file1:
    file1.write(str1)

with open("file2_" + str(pid), 'w') as file2:
    file2.write(str2)

with open("file3_" + str(pid), 'w') as file3:
    file3.write(str3)

# print the file contents to console
print(str1 + str2 + str3, end="")
# print two random numbers and their product to the console
print("{0[0]}\n{0[1]}\n{0[2]}".format(twoRandAndProduct()))
