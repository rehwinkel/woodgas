#include "render.h"

#include "shaders.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <system_error>
#include <cstring>
#include <cmath>

using namespace render;

Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

float Color::red() { return this->r; }
float Color::green() { return this->g; }
float Color::blue() { return this->b; }
float Color::alpha() { return this->a; }

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

AtlasEntry::AtlasEntry(size_t width, size_t height, int components,
                       const char *img_data)
    : width(width),
      height(height),
      components(components),
      img_data(img_data) {}

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
    GLenum color_format;
    switch (components) {
        case 4:
            color_format = GL_RGBA;
            break;
        case 3:
            color_format = GL_RGB;
            break;
        case 2:
            color_format = GL_RG;
            break;
        case 1:
            color_format = GL_RED;
            break;
        default:
            throw std::runtime_error(
                "impossible number of image color channels: " +
                std::to_string(components));
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                 color_format, GL_UNSIGNED_BYTE, img_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<TextureRef> Texture::create_atlas(std::vector<AtlasEntry> entries,
                                              bool interpolate) {
    if (entries.size() == 0) {
        throw std::runtime_error("can't create atlas with 0 textures");
    }
    size_t entry_width = entries[0].width;
    size_t entry_height = entries[0].height;
    int entry_components = entries[0].components;
    int16_t atlas_size = (int16_t)std::ceil(std::sqrt(entries.size()));
    size_t texture_width = entry_width * atlas_size;
    size_t texture_height = entry_height * atlas_size;
    char *texture_data =
        (char *)std::malloc(texture_width * texture_height * entry_components);
    for (size_t i = 0; i < entries.size(); i++) {
        AtlasEntry &entry = entries[i];
        if (entry.width != entry_width || entry.height != entry_height ||
            entry.components != entry_components) {
            throw std::runtime_error(
                "all atlas textures must have same dimensions");
        }
        size_t x = i % atlas_size;
        size_t y = i / atlas_size;
        for (size_t row = 0; row < entry_height; row++) {
            size_t dst_offset =
                ((row + y * entry_height) * texture_width + x * entry_width) *
                entry_components;
            size_t src_offset = row * entry_width * entry_components;
            std::memcpy(texture_data + dst_offset, entry.img_data + src_offset,
                        entry_width * entry_components);
        }
    }
    Texture tex(texture_width, texture_height, entry_components, texture_data,
                interpolate);
    std::vector<TextureRef> refs;
    for (int16_t i = 0; i < (int16_t)entries.size(); i++) {
        int16_t x = i % atlas_size;
        int16_t y = i / atlas_size;
        refs.push_back(render::TextureRef(tex, x, y, atlas_size, atlas_size));
    }
    return refs;
}

TextureRef::TextureRef(Texture texture)
    : texture(texture), size(-1, -1), offset(-1, -1) {}

TextureRef::TextureRef(Texture texture, int16_t x, int16_t y, int16_t width,
                       int16_t height)
    : texture(texture), size(width, height), offset(x, y) {}

bool TextureRef::operator==(const TextureRef &other) const {
    if (this->texture.get_texture() != other.texture.get_texture()) {
        return false;
    }
    return this->offset == other.offset && this->size == other.size;
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

GLuint Texture::get_texture() const { return this->texture; }

void Texture::cleanup() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &this->texture);
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<float> vertices, std::vector<float> uvs,
           std::vector<int> indices)
    : length((GLsizei)indices.size()) {
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

GLsizei Mesh::get_length() { return this->length; }

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
    this->view_uni = glGetUniformLocation(this->program, "view");
    this->atlas_uni = glGetUniformLocation(this->program, "atlas");
    this->start();
    glUniform4f(this->atlas_uni, 0, 0, 1, 1);
}

void QuadShader::set_transform(float *data, bool change_shader_state) {
    if (change_shader_state) {
        this->start();
    }
    glUniformMatrix4fv(this->transform_uni, 1, true, data);
    if (change_shader_state) {
        this->stop();
    }
}

void QuadShader::set_ortho(float *data, bool change_shader_state) {
    if (change_shader_state) {
        this->start();
    }
    glUniformMatrix4fv(this->ortho_uni, 1, true, data);
    if (change_shader_state) {
        this->stop();
    }
}

void QuadShader::set_view(float *data, bool change_shader_state) {
    if (change_shader_state) {
        this->start();
    }
    glUniformMatrix4fv(this->view_uni, 1, true, data);
    if (change_shader_state) {
        this->stop();
    }
}

void QuadShader::set_atlas(int16_t x, int16_t y, int16_t w, int16_t h,
                           bool change_shader_state) {
    if (change_shader_state) {
        this->start();
    }
    float tw = 1.0f / (float)w;
    float th = 1.0f / (float)h;
    glUniform4f(this->atlas_uni, tw * (float)x, th * (float)y, tw, th);
    if (change_shader_state) {
        this->stop();
    }
}

Renderer::Renderer(Window &window, logging::Logger &logger)
    : background(0, 0, 0, 0), logger(logger) {
    (void)(window);  // TODO: use window?
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
    this->set_background_color({1.0, 1.0, 1.0, 1.0});
    this->quad_shader = QuadShader();
    this->quad_shader.load_uniforms();
    this->upload_transform(render::Transform3D());
    this->upload_view(0, 0, 0);
}

void Renderer::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

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

void Renderer::upload_view(float x, float y, float z) {
    Transform3D transform = Transform3D().translate(-x, -y, -z);
    this->quad_shader.set_view(transform.get_data());
}

void Renderer::bind_texture(TextureRef &tex) {
    if (tex.offset.first >= 0 && tex.offset.second >= 0 &&
        tex.size.first >= 0 && tex.size.second >= 0) {
        this->quad_shader.set_atlas(tex.offset.first, tex.offset.second,
                                    tex.size.first, tex.size.second);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.texture.get_texture());
}

void Renderer::bind_texture(Texture &tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.get_texture());
}

void Renderer::draw_quad() {
    this->batch_draw_quad_begin();
    this->batch_draw_quad();
    this->batch_draw_quad_end();
}

void Renderer::batch_upload_transform(Transform3D &&tf) {
    this->quad_shader.set_transform(tf.get_data(), false);
}

void Renderer::batch_upload_transform(Transform3D &tf) {
    this->quad_shader.set_transform(tf.get_data(), false);
}
void Renderer::batch_bind_texture(TextureRef &tex) {
    if (tex.offset.first >= 0 && tex.offset.second >= 0 &&
        tex.size.first >= 0 && tex.size.second >= 0) {
        this->quad_shader.set_atlas(tex.offset.first, tex.offset.second,
                                    tex.size.first, tex.size.second, false);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.texture.get_texture());
}

void Renderer::batch_draw_quad_begin() {
    this->quad_shader.start();
    glBindVertexArray(this->quad.get_vao());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quad.get_indices());
}

void Renderer::batch_draw_quad_end() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    this->quad_shader.stop();
}

void Renderer::batch_draw_quad() {
    glDrawElements(GL_TRIANGLES, this->quad.get_length(), GL_UNSIGNED_INT,
                   nullptr);
}

void Renderer::set_background_color(Color &&color) {
    this->background = color;
    glClearColor(color.red(), color.green(), color.blue(), color.alpha());
}

Color &Renderer::get_background_color() { return this->background; }