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

Mesh::Mesh(std::vector<float> vertices)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &(*vertices.begin()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    this->vao = vao;
    this->vertex_vbo = vertex_vbo;
    this->length = vertices.size() / 3;
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

Renderer::Renderer(Window &window)
{
    this->quad = Mesh(std::vector<float>{-0.5, 0.5, 0, -0.5, -0.5, 0, 0.5, -0.5, 0, 0.5, -0.5, 0, 0.5, 0.5, 0, -0.5, 0.5, 0});
}

void Renderer::clear()
{
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw_quad()
{
    glBindVertexArray(this->quad.get_vao());
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, this->quad.get_length());
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}