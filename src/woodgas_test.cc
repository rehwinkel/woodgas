// #include <iostream>
// #include <lua.hpp>
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>
// #include <zlib.h>
// #include <AL/al.h>
// #include <AudioFile.h>

#include "render/render.h"
#include "input/input.h"

int main(int argc, char const *argv[])
{
    render::Window window(640, 480, "hey");
    render::Renderer renderer(window);
    input::Input input(window);

    while (window.is_open())
    {
        window.poll_inputs();

        renderer.clear();
        renderer.draw_quad();

        window.swap_buffers();
    }
    /*
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
    compress(nullptr, nullptr, nullptr, 0);
    stbi_load("test", nullptr, nullptr, nullptr, 4);
    ALuint buf;
    alGenBuffers(1, &buf);
    AudioFile<double> audiofile;
    std::cout << "hey" << std::endl;
    */
    return 0;
}
