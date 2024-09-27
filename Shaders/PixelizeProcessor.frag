#version 460

#extension GL_GOOGLE_include_directive : enable

#include "Common/PostProcessingPushConstantData.h"

layout (location = 0) in vec2 fragment_uv;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D post_processed_texture;

layout(push_constant) uniform PushConstant { PostProcessingPushConstantData push_constant_data; };

void main()
{
    if (push_constant_data.pixelize > 0)
    {
        vec2 quantized_fragment_uv = fragment_uv - mod(fragment_uv, 0.005);
        out_color = texture(post_processed_texture, quantized_fragment_uv);
    }
    else
    {
        out_color = texture(post_processed_texture, fragment_uv);
    }
}