## **Installing**

`pip install https://github.com/LandgrafHomyak/pyhp/archive/refs/tags/v0.0.0b2.tar.gz`

or link to wheel from [release](https://github.com/LandgrafHomyak/pyhp/releases/tag/v0.0.0b2) assets 

## **Using**

```python
import pyhp.executor import exec

source = r"""
<html>
<body>
    <?= "Hello, world!" ?>
    <br>
    <?python
        print("From PyHP!")
    ?>
    <br>
    <?= some_var ?>
</body>
</html>
"""

with open("hello_world.html", "wt") as fout:
    exec(source, fout, {"some_var": "passed text"})
```
Output will be:
```html
<html>
<body>
    Hello, world!
    <br>
    From PyHP!
    
    <br>
    passed text
</body>
</html>
```
