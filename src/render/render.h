// header for context creation and rendering API

#pragma once

#include <string>
#include <vector>

typedef unsigned int GLuint;

namespace render
{
    class Window
    {
        void *window;

    public:
        Window(int width, int height, std::string title);
        void poll_inputs();
        void swap_buffers();
        bool is_open();
    };

    class Mesh
    {
        GLuint vao;
        GLuint vertex_vbo;
        size_t length;

    public:
        Mesh();
        Mesh(std::vector<float> vertices);
        GLuint get_vao();
        size_t get_length();
        void cleanup();
    };

    class Shader
    {
        GLuint program;
        GLuint vertex_shader;
        GLuint fragment_shader;
        void check_for_error(GLuint shader);

    public:
        Shader();
        Shader(const char *vertex_shader_source, const char *fragment_shader_source);
        void start();
        void stop();
    };

    class Renderer
    {
        Mesh quad;
        Shader quad_shader;

    public:
        Renderer(Window &window);
        void clear();
        void draw_quad();
    };
} // namespace render
