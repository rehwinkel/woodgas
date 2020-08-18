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

Mesh::Mesh() {}

Mesh::Mesh(std::vector<float> vertices) : length(vertices.size() / 3)
{
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glGenBuffers(1, &this->vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &(*vertices.begin()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint Mesh::get_vao()
{
    return this->vao;
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
    this->quad = Mesh(std::vector<float>{-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0});
    this->quad_shader = Shader(quad_vertex_shader_source, quad_fragment_shader_source);
}

void Renderer::clear()
{
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw_quad()
{
    this->quad_shader.start();
    glBindVertexArray(this->quad.get_vao());
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, this->quad.get_length());
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    this->quad_shader.stop();
}