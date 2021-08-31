## **Installing**

`pip install git+https://github.com/LandgrafHomyak/Python-Hypertext-Preprocessor.git@v0.0.0b1`

## **Using**

```python
import pyhp.executor import exec_embed

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
    exec_embed(source, fout, {"some_var": "passed text"})
```
Output will be:
```html
<html>
<body>
    Hello, world!<br>
    From PyHP!
    <br>
    passed text
</body>
</html>
```
