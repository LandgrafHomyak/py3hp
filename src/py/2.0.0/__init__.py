import os
import os.path

path = os.path.abspath(os.path.dirname(__file__))
if path not in os.environ["PATH"]:
    os.environ["PATH"] = path + os.pathsep + os.getenv("PATH")

try:
    os.add_dll_directory(path)
except AttributeError:
    pass

del path
del os.path
del os
