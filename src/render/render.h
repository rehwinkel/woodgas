// header for context creation and rendering API

#pragma once

#include "../util/logging.h"

#include <string>
#include <vector>

typedef unsigned int GLuint;
typedef int GLint;

namespace render {
    class Window {
        void *window;
        logging::Logger &logger;

       public:
        Window(int width, int height, std::string title,
               logging::Logger &logger);
        ~Window();
        void poll_inputs();
        void swap_buffers();
        bool is_open();
        void *_get_window_ptr();
    };

    class Transform3D {
        float data[16];
        void matrix_multiply(float *a, float *b);

       public:
        Transform3D();
        Transform3D translate(float x, float y, float z);
        Transform3D rotate_x(float x);
        Transform3D rotate_y(float y);
        Transform3D rotate_z(float z);
        Transform3D scale(float x, float y, float z);
        float *get_data();
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
       protected:
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

    class QuadShader : public Shader {
        GLint transform_uni;
        GLint ortho_uni;

       public:
        QuadShader();
        void load_uniforms();
        void set_transform(float *data);
        void set_ortho(float *data);
    };

    class Renderer {
        Mesh quad;
        QuadShader quad_shader;
        logging::Logger &logger;

       public:
        Renderer(Window &window, logging::Logger &logger);
        void clear();
        void upload_transform(Transform3D &&tf);
        void upload_transform(Transform3D &tf);
        void upload_ortho(float left, float right, float bottom, float top,
                          float near, float far);
        void bind_texture(Texture &tex);
        void draw_quad();
    };
}  // namespace render
