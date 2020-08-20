#include "python.h"

#include <iostream>

#include <Python.h>

using namespace python;

PythonInterface::PythonInterface() {
    Py_Initialize();
    PyObject *module = PyImport_AddModule("__main__");
    PyObject *dict = PyModule_GetDict(module);
    if (PyRun_StringFlags("class Component:\n\tdef init():\n\t\tpass\n\tdef "
                          "update():\n\t\tpass",
                          Py_file_input, dict, dict, nullptr) == nullptr) {
        throw std::runtime_error(
            "failed to create component base python class");
    };
    PyObject *component_class =
        PyDict_GetItem(dict, PyUnicode_FromString("Component"));
    PyObject *v2 = PyRun_StringFlags("class MyComponent(Component):\n\tpass",
                                     Py_file_input, dict, dict, nullptr);
    PyObject *vals = PyDict_Items(dict);
    size_t l = PyList_Size(vals);
    for (size_t i = 0; i < l; i++) {
        PyObject *name = PyTuple_GetItem(PyList_GetItem(vals, i), 0);
        PyObject *clazz = PyTuple_GetItem(PyList_GetItem(vals, i), 1);
        if (clazz != component_class &&
            PyObject_IsInstance(clazz, (PyObject *)&PyType_Type) &&
            PyObject_IsSubclass(clazz, component_class)) {
            const char *s = PyUnicode_AsUTF8(name);
            std::cout << "class " << s << std::endl;
        }
    }
    Py_Finalize();
}