#version 460

layout (location = 0) in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(vec3(1.0, 0.0, 0.0), 1.0);
}