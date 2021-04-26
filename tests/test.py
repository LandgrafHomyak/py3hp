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
co = py3hp.compile(page)
print(*co, sep="\n")