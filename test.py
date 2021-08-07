from pyhp.parser import *
from pyhp.compiler import *

print()
print(align_code("   e;lkf\n  j\n      f"))

print()
r = "<?= erwerwerwer?>dfsdfsdfsdf<?python fmdlksdmflksdmfkmsdlfkm\nlfsslkdf?>sdfsdf<?=0?>"
print(*zip(parse(r), prepare(parse(r))), sep="\n")
print()
for x in parse(r):
    print(repr(x.value))
print()
for x in prepare(parse(r)):
    print(repr(x.value))

print()
print(repr(RAW), repr(TEXT))
print(RAW, TEXT)