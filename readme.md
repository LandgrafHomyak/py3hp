# **Installing**

`pip install git+https://github.com/LandgrafHomyak/Python-Hypertext-Preprocessor.git`

# **Using**

```python

# single file

import py3hp


page = py3hp.interpret_as_subprocess("""
    <?python3
        print("Hello world!")
        print(url_arguments.args)
        print(url_arguments.kwargs)
    ?> 
""")

with open("hello_world.txt", "wb") as fout:
    fout.write(page, "abc&def&g=7&h=8&i=9&j=10")
```
```python

# server

import py3hp


py3hp.serve_forever("Z:/www", 8080, "localhost")
```
