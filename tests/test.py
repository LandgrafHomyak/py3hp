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
print(*list(core.parse("sdndskfnsf<?3=print(20)?>0")))

print(*core.compile("sdndskfnsf<?3=print(20)?>0", "<test>"))
