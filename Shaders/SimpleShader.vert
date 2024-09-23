#version 460

#extension GL_GOOGLE_include_directive : enable

#include "Common/CameraUBO.h"
#include "Common/PushConstantData.h"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec3 fragment_normal;
layout (location = 2) out vec2 fragment_uv;

layout(set = 0, binding = 0) readonly uniform Camera { CameraUBO camera; };

layout(push_constant) uniform PushConstant { PushConstantData push_constant_data; };

void main()
{
    vec4 world_position = push_constant_data.model * vec4(position, 1.0);
    fragment_position = world_position.xyz;
    gl_Position = camera.projection * camera.view * world_position;

    fragment_normal = push_constant_data.normal * normal;
    fragment_uv = uv;
}