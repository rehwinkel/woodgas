#include "input.h"

using namespace input;

Input::Input(render::Window &window, logging::Logger &logger)
    : window(window), logger(logger) {}

bool Input::is_key_down(Key key) {
    return glfwGetKey((GLFWwindow *)this->window._get_window_ptr(), key);
}