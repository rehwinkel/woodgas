#include <Python.h>

#include "../../util/logging.h"

logging::Logger &get_logger(PyObject *module_obj) {
    PyObject *capsule =
        PyDict_GetItemString(PyModule_GetDict(module_obj), "p_logger");
    logging::Logger *logger =
        (logging::Logger *)PyCapsule_GetPointer(capsule, nullptr);
    return *logger;
}

void do_logging(int start_index, int elements, logging::LogLevel log_level,
                PyObject *self, PyObject *args, PyObject *kwargs) {
    const char *sep = " ";
    if (kwargs) {
        PyObject *sep_obj = PyDict_GetItemString(kwargs, "sep");
        if (sep_obj) {
            sep = PyUnicode_AsUTF8(sep_obj);
        }
    }
    logging::Logger &logger = get_logger(self);
    std::ostream &stream = logger.log_stream(log_level);
    for (int i = start_index; i < elements; i++) {
        PyObject *repr = PyObject_Repr(PyTuple_GetItem(args, i));
        stream << PyUnicode_AsUTF8(repr);
        Py_DECREF(repr);
        if (i + 1 < elements) {
            stream << sep;
        }
    }
    stream << logging::COLOR_RS << std::endl;
}

static PyObject *logger_log(PyObject *self, PyObject *args, PyObject *kwargs) {
    int tsz = PyTuple_Size(args);
    if (tsz < 2) {
        PyErr_Format(PyExc_TypeError,
                     "function takes at least 2 arguments (%zd given)", tsz);
        return NULL;
    }
    logging::LogLevel log_level =
        (logging::LogLevel)PyLong_AsLong(PyTuple_GetItem(args, 0));
    do_logging(1, tsz, log_level, self, args, kwargs);
    Py_RETURN_NONE;
}

static PyObject *logger_error(PyObject *self, PyObject *args,
                              PyObject *kwargs) {
    int tsz = PyTuple_Size(args);
    if (tsz < 1) {
        PyErr_Format(PyExc_TypeError,
                     "function takes at least 1 argument (%zd given)", tsz);
        return NULL;
    }
    do_logging(0, tsz, logging::LogLevel::ERROR, self, args, kwargs);
    Py_RETURN_NONE;
}

static PyObject *logger_warn(PyObject *self, PyObject *args, PyObject *kwargs) {
    int tsz = PyTuple_Size(args);
    if (tsz < 1) {
        PyErr_Format(PyExc_TypeError,
                     "function takes at least 1 argument (%zd given)", tsz);
        return NULL;
    }
    do_logging(0, tsz, logging::LogLevel::WARN, self, args, kwargs);
    Py_RETURN_NONE;
}

static PyObject *logger_info(PyObject *self, PyObject *args, PyObject *kwargs) {
    int tsz = PyTuple_Size(args);
    if (tsz < 1) {
        PyErr_Format(PyExc_TypeError,
                     "function takes at least 1 argument (%zd given)", tsz);
        return NULL;
    }
    do_logging(0, tsz, logging::LogLevel::INFO, self, args, kwargs);
    Py_RETURN_NONE;
}

static PyObject *logger_debug(PyObject *self, PyObject *args,
                              PyObject *kwargs) {
    int tsz = PyTuple_Size(args);
    if (tsz < 1) {
        PyErr_Format(PyExc_TypeError,
                     "function takes at least 1 argument (%zd given)", tsz);
        return NULL;
    }
    do_logging(0, tsz, logging::LogLevel::DEBUG, self, args, kwargs);
    Py_RETURN_NONE;
}

static PyObject *logger_set_log_level(PyObject *self, PyObject *args) {
    logging::Logger &logger = get_logger(self);
    logging::LogLevel lvl = (logging::LogLevel)PyLong_AsLong(args);
    logger.set_log_level(lvl);
    Py_RETURN_NONE;
}

static PyMethodDef logger_methods[] = {
    {"log", (PyCFunction)logger_log, METH_VARARGS | METH_KEYWORDS,
     "Logs all supplied objects to console with the given log level."},
    {"set_log_level", logger_set_log_level, METH_O,
     "Sets the log level of the logger"},
    {"error", (PyCFunction)logger_error, METH_VARARGS | METH_KEYWORDS,
     "Logs all supplied objects to console with ERROR log level."},
    {"warn", (PyCFunction)logger_warn, METH_VARARGS | METH_KEYWORDS,
     "Logs all supplied objects to console with WARN log level."},
    {"info", (PyCFunction)logger_info, METH_VARARGS | METH_KEYWORDS,
     "Logs all supplied objects to console with INFO log level."},
    {"debug", (PyCFunction)logger_debug, METH_VARARGS | METH_KEYWORDS,
     "Logs all supplied objects to console with DEBUG log level."},
    {NULL, NULL, 0, NULL}};

/*void set_log_level(LogLevel level);
    void log(std::string message, LogLevel level);
    std::ostream &log_stream(LogLevel level);
    void debug(std::string message);
    void info(std::string message);
    void warn(std::string message);
    void error(std::string message);
    std::ostream &debug_stream();
    std::ostream &info_stream();
    std::ostream &warn_stream();
    std::ostream &error_stream();*/

static struct PyModuleDef logger_module = {PyModuleDef_HEAD_INIT, "logger",
                                           nullptr, -1, logger_methods};