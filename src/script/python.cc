#include "python.h"

#include <Python.h>

using namespace python;

PythonInterface::PythonInterface() {
    Py_Initialize();
    PyRun_SimpleString("print('Hello World from Embedded Python!!!')");
    Py_Finalize();
}