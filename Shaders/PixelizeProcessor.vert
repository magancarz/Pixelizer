#version 460

#extension GL_GOOGLE_include_directive : enable

#include "Common/QuadData.h"

layout (location = 0) out vec2 fragment_uv;

void main()
{
    gl_Position = VERTICES[INDICES[NUM_OF_VERTICES * gl_InstanceIndex + gl_VertexIndex]];
    fragment_uv = TEXTURE_COORDS[INDICES[NUM_OF_VERTICES * gl_InstanceIndex + gl_VertexIndex]];
}