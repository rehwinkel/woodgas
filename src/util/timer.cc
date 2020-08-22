#include "timer.h"

#include <GLFW/glfw3.h>

using namespace timer;

Time::Time(render::Window &window) : prev_frame_time(glfwGetTime()) {}

double Time::current() { return glfwGetTime(); }

double Time::delta_time() { return glfwGetTime() - this->prev_frame_time; }

void Time::_frame_complete() { this->prev_frame_time = glfwGetTime(); }