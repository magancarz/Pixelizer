#ifndef COMMON_HOST_DEVICE
#define COMMON_HOST_DEVICE

#ifdef __cplusplus

#include <glm/glm.hpp>

using mat4 = glm::mat4;

#endif

struct CameraUBO
{
    mat4 projection;
    mat4 view;
};

#endif