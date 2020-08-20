#include "python.h"

#include <system_error>

using namespace python;

PythonComponent::PythonComponent(PyObject *init_function,
                                 PyObject *update_function)
    : init_function_obj(init_function), update_function_obj(update_function) {}

void PythonComponent::update() {
    PyObject_Call(this->update_function_obj, PyTuple_New(0), nullptr);
}

void PythonComponent::init() {
    PyObject_Call(this->init_function_obj, PyTuple_New(0), nullptr);
}

bool PythonComponent::is_unique() { return false; }

PythonInterface::PythonInterface() {
    Py_Initialize();
    PyObject *module = PyImport_AddModule("__main__");
    this->global_scope = PyModule_GetDict(module);
    if (PyRun_StringFlags("class Component:\n\tdef init():\n\t\tpass\n\tdef "
                          "update():\n\t\tpass",
                          Py_file_input, this->global_scope, this->global_scope,
                          nullptr) == nullptr) {
        throw std::runtime_error(
            "failed to create component base python class");
    };
    this->component_clazz =
        PyDict_GetItem(this->global_scope, PyUnicode_FromString("Component"));
}

PythonInterface::~PythonInterface() { Py_Finalize(); }

void PythonInterface::add_code(std::string py_source) {
    PyObject *v =
        PyRun_StringFlags(py_source.c_str(), Py_file_input, this->global_scope,
                          this->global_scope, nullptr);
}

std::map<std::string, PythonComponent> PythonInterface::load_components() {
    std::map<std::string, PythonComponent> components;
    PyObject *vals = PyDict_Items(this->global_scope);
    size_t l = PyList_Size(vals);
    for (size_t i = 0; i < l; i++) {
        PyObject *name = PyTuple_GetItem(PyList_GetItem(vals, i), 0);
        PyObject *clazz = PyTuple_GetItem(PyList_GetItem(vals, i), 1);
        if (clazz != this->component_clazz &&
            PyObject_IsInstance(clazz, (PyObject *)&PyType_Type) &&
            PyObject_IsSubclass(clazz, this->component_clazz)) {
            PyObject *init_function = PyObject_GetAttrString(clazz, "init");
            PyObject *update_function = PyObject_GetAttrString(clazz, "update");
            std::string name_str(PyUnicode_AsUTF8(name));
            components.insert(
                {name_str, PythonComponent(init_function, update_function)});
        }
    }
    return std::move(components);
}