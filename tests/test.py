import sys

import py3hp
print("py3hp", py3hp.version, "on", sys.platform)

import py3hp.core as core
print("-" * 100)
print(core.align("  sd;fsd\n jsndfsndf\n  fsdf\n jsd\n\n"))
print("-" * 100)
print(*core.parse("10\n<?3=avav?>kslmfk<?python3\n mfskldmlmsflkdmklsmfd?><?3=0?>klsdfm"))
print("-" * 100)
c = core.compile("ab\n<?3='ab'?>cdefgh<?python3\n print(' block')\n x = 'variable '?><?3=x?>ikjlmn\n", "<test>")
print(*c)
print("-" * 100)
core._exec(c)
print("-" * 100)
core.exec(c, dict(), sys.stdin, sys.stdout, sys.stderr)
print("-" * 100)
