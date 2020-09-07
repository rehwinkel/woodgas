#include "python.h"

#include "py_modules/render.cc"
#include "py_modules/logger.cc"

#include <system_error>
#include <marshal.h>

using namespace python;

PythonComponent::PythonComponent(PyObject *init_function,
                                 PyObject *update_function)
    : init_function_obj(init_function), update_function_obj(update_function) {}

PythonComponent::~PythonComponent() {
    Py_DECREF(this->init_function_obj);
    Py_DECREF(this->update_function_obj);
}

void PythonComponent::update() {
    PyObject_Call(this->update_function_obj, PyTuple_New(0), nullptr);
}

void PythonComponent::init() {
    PyObject_Call(this->init_function_obj, PyTuple_New(0), nullptr);
}

bool PythonComponent::is_unique() { return false; }

std::vector<unsigned char> python::compile_python(logging::Logger &logger,
                                                  std::string source,
                                                  std::string filename) {
    Py_Initialize();
    PyObject *code = Py_CompileStringExFlags(source.c_str(), filename.c_str(),
                                             Py_file_input, NULL, 2);
    if (!code) {
        logger.error("failed to parse or compile python code");
    }
    PyObject *encoded_code = PyMarshal_WriteObjectToString(code, 2);
    char *code_data = PyBytes_AsString(encoded_code);
    size_t encoded_size = PyBytes_Size(encoded_code);
    std::vector<unsigned char> data((unsigned char *)code_data,
                                    (unsigned char *)code_data + encoded_size);
    Py_DECREF(encoded_code);
    Py_DECREF(code);
    Py_Finalize();
    return std::move(data);
}

PythonInterface::PythonInterface(logging::Logger &logger,
                                 render::Renderer &renderer)
    : logger(logger) {
    logger.debug("initializing python...");
    Py_Initialize();
    PyObject *module = PyImport_AddModule("__main__");
    this->global_scope = PyModule_GetDict(module);
    PyObject *run_result;
    if (!(run_result = PyRun_StringFlags(
              "class Component:\n\tdef init():\n\t\tpass\n\tdef "
              "update():\n\t\tpass",
              Py_file_input, this->global_scope, this->global_scope,
              nullptr))) {
        throw std::runtime_error(
            "failed to create component base python class");
    };
    Py_DECREF(run_result);
    this->component_clazz =
        PyDict_GetItemString(this->global_scope, "Component");
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
    Py_DECREF(PyRun_StringFlags(py_source.c_str(), Py_file_input,
                                this->global_scope, this->global_scope,
                                nullptr));
}

void PythonInterface::add_code(asset::Generic &py_code) {
    logger.debug("running python code segment...");
    PyObject *code = PyMarshal_ReadObjectFromString((char *)py_code.get_data(),
                                                    py_code.size());
    Py_DECREF(PyEval_EvalCode(code, this->global_scope, this->global_scope));
    Py_DECREF(code);
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
            components.emplace(name_str,
                               PythonComponent(init_function, update_function));
        }
    }
    Py_DECREF(vals);
    return std::move(components);
}

void PythonInterface::print_error() {
    if (PyErr_Occurred()) {
        std::ostream &str = this->logger.error_stream();
        str << "python error:" << std::endl;
        PyErr_Print();
        str << logging::COLOR_RS << std::endl;
    }
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