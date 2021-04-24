import _py3hp as py3hp

b = py3hp.compile(
"""
<html>
<head></head>
<body>
<?python3 print("<span></span>") ?>
<?python3
  print("<div>")
  print("</div>")
?>    
</body>
</html>
"""
)
exec(b)
