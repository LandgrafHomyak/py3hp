import os
import os.path

path = os.path.abspath(os.path.dirname(__file__))
if path not in os.getenv("PATH"):
    os.putenv("PATH", path + os.pathsep + os.getenv("PATH"))

del path
del os.path
del os


