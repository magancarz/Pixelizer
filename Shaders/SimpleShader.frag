#version 460

layout (location = 0) in vec3 fragment_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

layout(set = 1, binding = 0) uniform sampler2D diffuse_texture;

const vec3 light_position = vec3(5.0, 0.0, 0.0);

void main()
{
    vec3 basic_color = texture(diffuse_texture, fragment_uv).xyz;
    vec3 to_light_vector = normalize(light_position - fragment_position);
    basic_color *= max(0.02, dot(to_light_vector, fragment_normal));
    out_color = vec4(basic_color, 1.0);
}