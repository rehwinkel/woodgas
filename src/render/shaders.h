const char *quad_vertex_shader_source = R"glsl(
#version 150 core

in vec3 position;
in vec2 uv;
out vec4 pass_color;

void main()
{
    gl_Position = vec4(position * 0.5, 1.0);
    pass_color = vec4(uv, 0.0, 1.0);
}
)glsl";

const char *quad_fragment_shader_source = R"glsl(
#version 150 core

in vec4 pass_color;
out vec4 out_color;

uniform sampler2D color_tex;

void main()
{
    out_color = texture(color_tex, pass_color.xy);
}
)glsl";