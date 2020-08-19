#include "render.h"

#include "shaders.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <system_error>
#include <cstring>

using namespace render;

Window::Window(int width, int height, std::string title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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

Texture::Texture(size_t width, size_t height, int components, const char *img_data, bool interpolate)
{
    //TODO: use components
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::get_texture()
{
    return this->texture;
}

void Texture::cleanup()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &this->texture);
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uvs, std::vector<int> indices) : length(indices.size())
{
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    glGenBuffers(1, &this->vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &(*vertices.begin()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &this->uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), &(*uvs.begin()), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &this->index_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &(*indices.begin()), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint Mesh::get_vao()
{
    return this->vao;
}

GLuint Mesh::get_indices()
{
    return this->index_vbo;
}

size_t Mesh::get_length()
{
    return this->length;
}

void Mesh::cleanup()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &this->vertex_vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &this->vao);
}

void Shader::check_for_error(GLuint shader)
{
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE)
    {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

        glDeleteShader(shader);
        throw std::runtime_error(std::string(error_log.begin(), error_log.end()));
    }
}

Shader::Shader() {}

Shader::Shader(const char *vertex_shader_source, const char *fragment_shader_source)
{
    this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(this->vertex_shader);
    this->check_for_error(this->vertex_shader);
    this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(this->fragment_shader);
    this->check_for_error(this->fragment_shader);

    this->program = glCreateProgram();
    glAttachShader(this->program, this->vertex_shader);
    glAttachShader(this->program, this->fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);
}

void Shader::start()
{
    glUseProgram(this->program);
}

void Shader::stop()
{
    glUseProgram(0);
}

Renderer::Renderer(Window &window)
{
    this->quad = Mesh(
        std::vector<float>{
            -1,
            1,
            0,
            -1,
            -1,
            0,
            1,
            -1,
            0,
            1,
            1,
            0,
        },
        std::vector<float>{0, 0, 0, 1, 1, 1, 1, 0},
        std::vector<int>{0, 1, 2, 2, 3, 0});
    this->quad_shader = Shader(quad_vertex_shader_source, quad_fragment_shader_source);
}

void Renderer::clear()
{
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::bind_texture(Texture &tex)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.get_texture());
}

void Renderer::draw_quad()
{
    this->quad_shader.start();
    glBindVertexArray(this->quad.get_vao());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quad.get_indices());
    glDrawElements(GL_TRIANGLES, this->quad.get_length(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    this->quad_shader.stop();
}