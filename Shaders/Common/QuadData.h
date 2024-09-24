#ifndef QUAD_DATA_HOST_DEVICE
#define QUAD_DATA_HOST_DEVICE

const vec4 VERTICES[4] = vec4[4]
(
    vec4(-1, -1,  0,  1),
    vec4( 1, -1,  0,  1),
    vec4( 1,  1,  0,  1),
    vec4(-1,  1,  0,  1)
);

const vec2 TEXTURE_COORDS[4] = vec2[4]
(
    vec2(0, 0),
    vec2(1, 0),
    vec2(1, 1),
    vec2(0, 1)
);

const uint INDICES[6] = uint[6](0, 1, 2, 2, 3, 0);

const int NUM_OF_VERTICES = 3;

#endif