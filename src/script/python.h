// header for python integration API

#pragma once

#include "../core/core.h"

#include <Python.h>

namespace python {
    class PythonComponent : public core::Component {
        PyObject *init_function_obj;
        PyObject *update_function_obj;

       public:
        PythonComponent(PyObject *init_function_obj,
                        PyObject *update_function_obj);
        virtual void update();
        virtual void init();
        virtual bool is_unique();
    };

    class PythonInterface {
        PyObject *global_scope;
        PyObject *component_clazz;

       public:
        PythonInterface();
        ~PythonInterface();
        void add_code(std::string py_source);
        std::map<std::string, PythonComponent> load_components();
    };
}