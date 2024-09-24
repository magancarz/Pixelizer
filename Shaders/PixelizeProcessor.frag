#version 460

layout (location = 0) in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D post_processed_texture;

void main()
{
    out_color = texture(post_processed_texture, fragment_uv);
}