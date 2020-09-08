#include <Python.h>

#include "../../render/render.h"

render::Renderer &get_renderer(PyObject *module_obj) {
    PyObject *capsule =
        PyDict_GetItemString(PyModule_GetDict(module_obj), "p_render");
    render::Renderer *renderer =
        (render::Renderer *)PyCapsule_GetPointer(capsule, nullptr);
    return *renderer;
}

render::Window &get_window(PyObject *module_obj) {
    PyObject *capsule =
        PyDict_GetItemString(PyModule_GetDict(module_obj), "p_window");
    render::Window *window =
        (render::Window *)PyCapsule_GetPointer(capsule, nullptr);
    return *window;
}

static PyObject *render_draw_quad(PyObject *self, PyObject *args) {
    render::Renderer &renderer = get_renderer(self);
    renderer.draw_quad();
    Py_RETURN_NONE;
}

static PyObject *render_clear(PyObject *self, PyObject *args) {
    render::Renderer &renderer = get_renderer(self);
    renderer.clear();
    Py_RETURN_NONE;
}

static PyObject *render_swap_buffers(PyObject *self, PyObject *args) {
    render::Window &window = get_window(self);
    window.swap_buffers();
    Py_RETURN_NONE;
}

static PyObject *render_poll_inputs(PyObject *self, PyObject *args) {
    render::Window &window = get_window(self);
    window.poll_inputs();
    Py_RETURN_NONE;
}

static PyObject *render_is_window_open(PyObject *self, PyObject *args) {
    render::Window &window = get_window(self);
    return PyBool_FromLong(window.is_open());
}

static PyObject *render_upload_transform(PyObject *self, PyObject *args[],
                                         ssize_t length) {
    if (length != 9) {
        return nullptr;
    }
    double x = PyFloat_AsDouble(args[0]);
    double y = PyFloat_AsDouble(args[1]);
    double z = PyFloat_AsDouble(args[2]);
    double rx = PyFloat_AsDouble(args[3]);
    double ry = PyFloat_AsDouble(args[4]);
    double rz = PyFloat_AsDouble(args[5]);
    double sx = PyFloat_AsDouble(args[6]);
    double sy = PyFloat_AsDouble(args[7]);
    double sz = PyFloat_AsDouble(args[8]);
    render::Renderer &renderer = get_renderer(self);
    renderer.upload_transform(render::Transform3D()
                                  .translate(x, y, z)
                                  .rotate_x(rx)
                                  .rotate_y(ry)
                                  .rotate_z(rz)
                                  .scale(sx, sy, sz));
    Py_RETURN_NONE;
}

static PyObject *render_upload_ortho(PyObject *self, PyObject *args[],
                                     ssize_t length) {
    if (length != 6) {
        return nullptr;
    }
    double left = PyFloat_AsDouble(args[0]);
    double right = PyFloat_AsDouble(args[1]);
    double bottom = PyFloat_AsDouble(args[2]);
    double top = PyFloat_AsDouble(args[3]);
    double near = PyFloat_AsDouble(args[4]);
    double far = PyFloat_AsDouble(args[5]);
    render::Renderer &renderer = get_renderer(self);
    renderer.upload_ortho(left, right, bottom, top, near, far);
    Py_RETURN_NONE;
}

static PyMethodDef render_methods[] = {
    {"draw_quad", render_draw_quad, METH_NOARGS, "Draws a quad."},
    {"swap_buffers", render_swap_buffers, METH_NOARGS,
     "Swaps the render buffers."},
    {"poll_inputs", render_poll_inputs, METH_NOARGS, "Polls all input events."},
    {"is_window_open", render_is_window_open, METH_NOARGS,
     "Checks if the window should stay open."},
    {"clear", render_clear, METH_NOARGS,
     "Clears the screen with a color."},
    {"upload_transform", (PyCFunction)render_upload_transform, METH_FASTCALL,
     "Uploads a model transformation matrix to the shader."},
    {"upload_orthographic", (PyCFunction)render_upload_ortho, METH_FASTCALL,
     "Uploads an orthographic view matrix to the shader."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef render_module = {PyModuleDef_HEAD_INIT, "render",
                                           nullptr, -1, render_methods};