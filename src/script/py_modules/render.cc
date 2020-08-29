#include <Python.h>

#include "../../render/render.h"

static PyObject *render_draw_quad(PyObject *self, PyObject *args) {
    PyObject *p_render =
        PyDict_GetItemString(PyModule_GetDict(self), "p_render");
    render::Renderer *renderer =
        (render::Renderer *)PyCapsule_GetPointer(p_render, nullptr);
    renderer->draw_quad();
    Py_RETURN_NONE;
}

static PyMethodDef render_methods[] = {
    {"draw_quad", render_draw_quad, METH_VARARGS, "Draws a quad."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef render_module = {PyModuleDef_HEAD_INIT, "render",
                                           nullptr, -1, render_methods};