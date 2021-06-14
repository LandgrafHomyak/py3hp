import py3hp.core as core
print("-" * 100)
print(core.align("  sd;fsd\n jsndfsndf\n  fsdf\n jsd\n\n"))
print("-" * 100)
print(*core.parse("10\n<?3=avav?>kslmfk<?python3\n mfskldmlmsflkdmklsmfd?><?3=0?>klsdfm"))
print("-" * 100)
print(*core.compile("ab\n<?3=avav?>cdefgh<?python3\n mfskldmlmsflkdmklsmfd?><?3=0?>ikjlmn", "<test>"))
print("-" * 100)