#version 460

#extension GL_GOOGLE_include_directive : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec2 fragment_uv;

#include "Common/CameraUBO.h"

layout(set = 0, binding = 0) readonly uniform Camera { CameraUBO camera; };

void main()
{
    gl_Position = camera.projection * camera.view * vec4(position, 1.0);
    fragment_uv = uv;
}