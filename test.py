from pyhp.parser import *
from pyhp.compiler import *

print()
print(align_code("   e;lkf\n  j\n      f"))

print()
print(*(parse("<?=erwerwerwer?>dfsdfsdfsdf<?python fmdlksdmflksdmfkmsdlfkm\nlfsslkdf?>sdfsdf")))

print()
print(repr(RAW), repr(TEXT))
print(RAW, TEXT)