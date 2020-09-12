// header for context creation and rendering API

#pragma once

#include "../util/logging.h"

#include <string>
#include <vector>
#include <array>
#include <optional>

typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;

namespace render {
    class Color {
        float r, g, b, a;

       public:
        Color(float r, float g, float b, float a);
        float red();
        float green();
        float blue();
        float alpha();
    };

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
        std::array<float, 16> data;
        void matrix_multiply(std::array<float, 16> &a,
                             std::array<float, 16> &b);

       public:
        Transform3D();
        Transform3D translate(float x, float y, float z);
        Transform3D rotate_x(float x);
        Transform3D rotate_y(float y);
        Transform3D rotate_z(float z);
        Transform3D scale(float x, float y, float z);
        float *get_data();
    };

    class AtlasEntry {
       public:
        size_t width;
        size_t height;
        int components;
        const char *img_data;
        AtlasEntry(size_t width, size_t height, int components,
                   const char *img_data);
    };

    class Texture {
        GLuint texture;

       public:
        Texture(size_t width, size_t height, int components,
                const char *img_data, bool interpolate = false);
        static Texture create_atlas(std::vector<AtlasEntry> entires,
                                    bool interpolate = false);
        GLuint get_texture();
        void cleanup();
    };

    class TextureRef {
       public:
        Texture texture;
        std::optional<std::pair<size_t, size_t>> size;
        std::optional<std::pair<size_t, size_t>> offset;
        TextureRef(Texture texture);
        TextureRef(Texture texture, size_t x, size_t y, size_t width,
                   size_t height);
    };

    class Mesh {
        GLuint vao;
        GLuint vertex_vbo;
        GLuint uv_vbo;
        GLuint index_vbo;
        GLsizei length;

       public:
        Mesh();
        Mesh(std::vector<float> vertices, std::vector<float> uvs,
             std::vector<int> indices);
        GLuint get_vao();
        GLuint get_indices();
        GLsizei get_length();
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
        Color background;
        QuadShader quad_shader;
        logging::Logger &logger;

       public:
        Renderer(Window &window, logging::Logger &logger);
        void clear();
        void upload_transform(Transform3D &&tf);
        void upload_transform(Transform3D &tf);
        void upload_ortho(float left, float right, float bottom, float top,
                          float near, float far);
        void set_background_color(Color &&color);
        Color &get_background_color();
        void bind_texture(TextureRef &tex);
        void bind_texture(Texture &tex);
        void draw_quad();
    };
}  // namespace render
