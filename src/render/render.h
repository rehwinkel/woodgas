// header for context creation and rendering API

#pragma once

#include <string>
#include <vector>

typedef unsigned int GLuint;

namespace render {
    class Window {
        void *window;

       public:
        Window(int width, int height, std::string title);
        void poll_inputs();
        void swap_buffers();
        bool is_open();
    };

    class Texture {
        GLuint texture;

       public:
        Texture(size_t width, size_t height, int components,
                const char *img_data, bool interpolate = false);
        GLuint get_texture();
        void cleanup();
    };

    class Mesh {
        GLuint vao;
        GLuint vertex_vbo;
        GLuint uv_vbo;
        GLuint index_vbo;
        size_t length;

       public:
        Mesh();
        Mesh(std::vector<float> vertices, std::vector<float> uvs,
             std::vector<int> indices);
        GLuint get_vao();
        GLuint get_indices();
        size_t get_length();
        void cleanup();
    };

    class Shader {
        GLuint program;
        GLuint vertex_shader;
        GLuint fragment_shader;
        void check_for_error(GLuint shader);

       public:
        Shader();
        Shader(const char *vertex_shader_source,
               const char *fragment_shader_source);
        void start();
        void stop();
    };

    class Renderer {
        Mesh quad;
        Shader quad_shader;

       public:
        Renderer(Window &window);
        void clear();
        void bind_texture(Texture &tex);
        void draw_quad();
    };
}  // namespace render
