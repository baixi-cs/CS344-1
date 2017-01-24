import io
import os
import random

def tenRandomChar ():
  str = ""
  for x in range(0,10):
    str += chr(random.randint(97,122))
  return str

def twoRandAndSum ():
  rand1 = random.randint(1,42)
  rand2 = random.randint(1,42)
  return rand1, rand2, rand1 + rand2


random.seed()
pid = os.getpid()
file1 = open("file1" + str(pid), 'w')
file2 = open("file2" + str(pid), 'w')
file3 = open("file3" + str(pid), 'w')

str1 = tenRandomChar() + "\n"
str2 = tenRandomChar() + "\n"
str3 = tenRandomChar() + "\n"

file1.write(str1)
file2.write(str2)
file3.write(str3)

print(str1 + str2 + str3[:-1])
print("{0[0]}\n{0[1]}\n{0[2]}\n".format(twoRandAndSum()))





