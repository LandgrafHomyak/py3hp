import py3hp.core as core
import sys

print(dir(core))
print(core.align("""                 o
   aer

     er
    ds
   dsfdfds

   o"""))
print(core.RAW)
print(*core.parse("3rewwe<?3= ?>"))