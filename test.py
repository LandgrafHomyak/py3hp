import sys
from pyhp.parser import *
from pyhp.compiler import *
from pyhp.executor import *

print()
print(align_code("   e;lkf\n  j\n      f"))

print()
r = "<?= a ?> inner <?python print(b, end='') ?> outer <?= ';' ?>"
print(*map(lambda z: "\n".join(map(repr, z)), zip(parse(r), prepare(parse(r)), compiler_iterator(prepare(parse(r))))), sep="\n\n")
print()
for x in parse(r):
    print(repr(x.value))
print()
for x in prepare(parse(r)):
    print(repr(x.value))
print()
for x in compiler_iterator(prepare(parse(r))):
    print(repr(x.value))

print()
print(repr(RAW), repr(TEXT))
print(RAW, TEXT)

print()
g = {"a": "<{", "b": "}>"}
exec_embed(compiler_iterator(prepare(parse(r))), sys.stdout, g)
print()
exec_embed(r, sys.stdout, g)
