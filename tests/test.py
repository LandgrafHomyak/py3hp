import py3hp.core as core

page = b"""
abc  
<?3= "inline" ?>
def
<?python3
    print("block")
?>
ghi
"""
co = core.compile(page)


class readio:
    def read(self):
        return "1234"


print(co, sep="\n")
print(*co, sep="\n")

import json

json.load
