// header for python integration API

#pragma once

#include "../core/core.h"
#include "../render/render.h"
#include "../util/logging.h"
#include "../asset/asset.h"

#include <Python.h>

namespace python {
    PyObject *load_compiled_python(std::vector<unsigned char> data);
    std::vector<unsigned char> compile_python(logging::Logger &logger,
                                              std::string source,
                                              std::string filename);

    class PythonComponent : public core::Component {
        PyObject *init_function_obj;
        PyObject *update_function_obj;

       public:
        PythonComponent(PyObject *init_function_obj,
                        PyObject *update_function_obj);
        virtual ~PythonComponent();
        virtual void update(core::Interface &interface);
        virtual void init(core::Interface &interface);
        virtual bool is_unique();
    };

    class PythonInterface {
        PyObject *global_scope;
        PyObject *component_clazz;
        logging::Logger &logger;
        PyObject *create_render_module(render::Window &window,
                                       render::Renderer &renderer);
        PyObject *create_logger_module(logging::Logger &renderer);

       public:
        PythonInterface(logging::Logger &logger, render::Window &window,
                        render::Renderer &renderer);
        ~PythonInterface();
        void add_code(std::string py_source);
        void add_code(asset::Generic &py_code);
        std::map<std::string, PythonComponent> load_components();
        void start_main();
        void print_error();
    };
}