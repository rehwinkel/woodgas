#include "render.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <system_error>

using namespace render;

Window::Window(int width, int height, std::string title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize GLFW");
    }

    this->window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent((GLFWwindow *)this->window);
    if (!gladLoadGL())
    {
        throw std::runtime_error("failed to initialize GLAD");
    }
}

bool Window::is_open()
{
    return !glfwWindowShouldClose((GLFWwindow *)this->window);
}

void Window::poll_inputs()
{
    glfwPollEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers((GLFWwindow *)this->window);
}

Renderer::Renderer(Window &window) {}

void Renderer::clear()
{
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}