#include <Python.h>
#include <PyHP.h>
#include <PyHP/modules.h>

void main(void)
{
    Py_Initialize();
    PyHP_Init();
    PyObject *x = PyDict_New();
    PyDict_SetItemString(x, "exec_embed", PyObject_GetAttrString(PyHPInit_executor(), "exec_embed"));
    PyRun_String("import sys; exec_embed(\"\"\"<?python\n"
                 "    print(\"hello igor\")\n"
                 "?>\"\"\", sys.stdout)", Py_file_input, x, x);
    Py_Finalize();
}