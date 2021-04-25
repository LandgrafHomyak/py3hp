import _py3hp as py3hp

page = """
abc
<?3= "inline" ?>
def
<?python3
    print("block")
?>
ghi
"""

co = py3hp.compile_s(page)
print(*map(repr, co), sep="\n")

print("-----------------")

print(eval(co[1]))
exec(co[3])
