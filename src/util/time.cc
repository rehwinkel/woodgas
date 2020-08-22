#include "time.h"

#include <GLFW/glfw3.h>

using namespace time;

Time::Time() : prev_frame_time(glfwGetTime()) {}

double Time::current() { return glfwGetTime(); }

double Time::delta_time() { return glfwGetTime() - this->prev_frame_time; }

void Time::_frame_complete() { this->prev_frame_time = glfwGetTime(); }