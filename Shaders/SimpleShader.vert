#version 460

#extension GL_GOOGLE_include_directive : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec3 fragment_normal;
layout (location = 2) out vec2 fragment_uv;

#include "Common/CameraUBO.h"

layout(set = 0, binding = 0) readonly uniform Camera { CameraUBO camera; };

void main()
{
    fragment_position = position;
    gl_Position = camera.projection * camera.view * vec4(fragment_position, 1.0);
    fragment_normal = normal;
    fragment_uv = uv;
}