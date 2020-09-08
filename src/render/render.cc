#include "render.h"

#include "shaders.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <system_error>
#include <cstring>
#include <cmath>

using namespace render;

Window::Window(int width, int height, std::string title,
               logging::Logger &logger)
    : logger(logger) {
    logger.debug("initializing GLFW...");
    if (!glfwInit()) {
        logger.error("failed to initialize GLFW");
        throw std::runtime_error("failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    logger.debug("creating window...");
    this->window =
        glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (this->window == nullptr) {
        if (glfwGetError(nullptr) == GLFW_VERSION_UNAVAILABLE) {
            logger.error(
                "failed to create window because OpenGL 3.0 is not supported");
        } else {
            logger.error_stream()
                << "failed to create window with GLFW-Error-Code "
                << glfwGetError(nullptr) << logging::COLOR_RS << std::endl;
        }
        throw std::runtime_error("failed to create window");
    }
    glfwMakeContextCurrent((GLFWwindow *)this->window);
    logger.debug("loading GLAD...");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger.error("failed to load GLAD");
        throw std::runtime_error("failed to initialize GLAD");
    }
}

Window::~Window() {
    logger.debug("terminating GLFW...");
    glfwTerminate();
}

bool Window::is_open() {
    return !glfwWindowShouldClose((GLFWwindow *)this->window);
}

void Window::poll_inputs() { glfwPollEvents(); }

void Window::swap_buffers() { glfwSwapBuffers((GLFWwindow *)this->window); }

void *Window::_get_window_ptr() { return this->window; }

Texture::Texture(size_t width, size_t height, int components,
                 const char *img_data, bool interpolate) {
    // TODO: use components
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    interpolate ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    interpolate ? GL_LINEAR : GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Transform3D::Transform3D()
    : data{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {}

void Transform3D::matrix_multiply(std::array<float, 16> &a,
                                  std::array<float, 16> &b) {
    std::array<float, 16> out;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0;
            for (int i = 0; i < 4; i++) {
                float a_val = a[row * 4 + i];
                float b_val = b[col + i * 4];
                sum += a_val * b_val;
            }
            out[row * 4 + col] = sum;
        }
    }
    std::memcpy(a.data(), out.data(), 16 * sizeof(float));
}

Transform3D Transform3D::translate(float x, float y, float z) {
    std::array<float, 16> tm = {
        1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1,
    };
    this->matrix_multiply(this->data, tm);
    return *this;
}

Transform3D Transform3D::rotate_x(float x) {
    float c = std::cos(x);
    float s = std::sin(x);
    std::array<float, 16> tm = {
        1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1,
    };
    this->matrix_multiply(this->data, tm);
    return *this;
}

Transform3D Transform3D::rotate_y(float y) {
    float c = std::cos(y);
    float s = std::sin(y);
    std::array<float, 16> tm = {
        c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1,
    };
    this->matrix_multiply(this->data, tm);
    return *this;
}

Transform3D Transform3D::rotate_z(float z) {
    float c = std::cos(z);
    float s = std::sin(z);
    std::array<float, 16> tm = {
        c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    };
    this->matrix_multiply(this->data, tm);
    return *this;
}

Transform3D Transform3D::scale(float x, float y, float z) {
    std::array<float, 16> tm = {
        x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1,
    };
    this->matrix_multiply(this->data, tm);
    return *this;
}

float *Transform3D::get_data() { return this->data.data(); }

GLuint Texture::get_texture() { return this->texture; }

void Texture::cleanup() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &this->texture);
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uvs,
           std::vector<int> indices)
    : length(indices.size()) {
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    glGenBuffers(1, &this->vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 &(*vertices.begin()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &this->uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), &(*uvs.begin()),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &this->index_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int),
                 &(*indices.begin()), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint Mesh::get_vao() { return this->vao; }

GLuint Mesh::get_indices() { return this->index_vbo; }

size_t Mesh::get_length() { return this->length; }

void Mesh::cleanup() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &this->vertex_vbo);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &this->vao);
}

void Shader::check_for_error(GLuint shader) {
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == GL_FALSE) {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

        glDeleteShader(shader);
        throw std::runtime_error(
            std::string(error_log.begin(), error_log.end()));
    }
}

Shader::Shader() {}

Shader::Shader(const char *vertex_shader_source,
               const char *fragment_shader_source) {
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

void Shader::start() { glUseProgram(this->program); }

void Shader::stop() { glUseProgram(0); }

QuadShader::QuadShader()
    : Shader(quad_vertex_shader_source, quad_fragment_shader_source) {}

void QuadShader::load_uniforms() {
    glLinkProgram(this->program);
    this->transform_uni = glGetUniformLocation(this->program, "transform");
    this->ortho_uni = glGetUniformLocation(this->program, "ortho");
}

void QuadShader::set_transform(float *data) {
    this->start();
    glUniformMatrix4fv(this->transform_uni, 1, true, data);
    this->stop();
}

void QuadShader::set_ortho(float *data) {
    this->start();
    glUniformMatrix4fv(this->ortho_uni, 1, true, data);
    this->stop();
}

Renderer::Renderer(Window &window, logging::Logger &logger) : logger(logger) {
    logger.debug("creating quad mesh...");
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
    logger.debug("creating quad shader...");
    this->quad_shader = QuadShader();
    this->quad_shader.load_uniforms();
}

void Renderer::clear() {
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::upload_transform(Transform3D &&tf) {
    this->quad_shader.set_transform(tf.get_data());
}

void Renderer::upload_transform(Transform3D &tf) {
    this->quad_shader.set_transform(tf.get_data());
}

void Renderer::upload_ortho(float left, float right, float bottom, float top,
                            float near, float far) {
    float data[16] = {
        2.0f / (right - left),
        0,
        0,
        -(right + left) / (right - left),
        0,
        2.0f / (top - bottom),
        0,
        -(top + bottom) / (top - bottom),
        0,
        0,
        2.0f / (far - near),
        -(far + left) / (far - near),
        0,
        0,
        0,
        1,
    };
    this->quad_shader.set_ortho(data);
}

void Renderer::bind_texture(Texture &tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.get_texture());
}

void Renderer::draw_quad() {
    this->quad_shader.start();
    glBindVertexArray(this->quad.get_vao());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quad.get_indices());
    glDrawElements(GL_TRIANGLES, this->quad.get_length(), GL_UNSIGNED_INT,
                   nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    this->quad_shader.stop();
}