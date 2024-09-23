#ifndef PUSH_CONSTANT_DATA_HOST_DEVICE
#define PUSH_CONSTANT_DATA_HOST_DEVICE

#ifdef __cplusplus

#include <glm/glm.hpp>

using mat4 = glm::mat4;
using mat3 = glm::mat3;

#endif

struct PushConstantData
{
    mat4 model;
    mat3 normal;
};

#endif