#include "python.h"

#include "py_modules/render.cc"
#include "py_modules/logger.cc"

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

PythonInterface::PythonInterface(logging::Logger &logger,
                                 render::Renderer &renderer)
    : logger(logger) {
    logger.debug("initializing python...");
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
    PyDict_SetItemString(this->global_scope, "render",
                         this->create_render_module(renderer));
    PyDict_SetItemString(this->global_scope, "logger",
                         this->create_logger_module(logger));
}

PyObject *PythonInterface::create_render_module(render::Renderer &renderer) {
    PyObject *module = PyModule_Create(&render_module);
    PyDict_SetItemString(PyModule_GetDict(module), "p_render",
                         PyCapsule_New(&renderer, nullptr, nullptr));
    return module;
}

PyObject *PythonInterface::create_logger_module(logging::Logger &logger) {
    PyObject *module = PyModule_Create(&logger_module);
    PyDict_SetItemString(PyModule_GetDict(module), "p_logger",
                         PyCapsule_New(&logger, nullptr, nullptr));
    PyModule_AddIntConstant(module, "ERROR", logging::LogLevel::ERROR);
    PyModule_AddIntConstant(module, "WARN", logging::LogLevel::WARN);
    PyModule_AddIntConstant(module, "INFO", logging::LogLevel::INFO);
    PyModule_AddIntConstant(module, "DEBUG", logging::LogLevel::DEBUG);
    return module;
}

PythonInterface::~PythonInterface() { Py_Finalize(); }

void PythonInterface::add_code(std::string py_source) {
    logger.debug("running python code segment...");
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

void PythonInterface::start_main() {
    logger.debug("starting python main...");
    PyObject *main = PyDict_GetItemString(this->global_scope, "main");
    if (main) {
        PyObject_Call(main, Py_None, Py_None);
    } else {
        logger.error("missing python main");
    }
}