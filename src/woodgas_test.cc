#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <lua.hpp>

int main(int argc, char const *argv[])
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize GLEW");
    }
    GLFWwindow *window = glfwCreateWindow(640, 480, "Title", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (!gladLoadGL())
    {
        throw std::runtime_error("failed to initialize GLAD");
    }

    glClearColor(1.0, 0.0, 1.0, 1.0);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }
    lua_close(nullptr);
    std::cout << "hey" << std::endl;
    return 0;
}
