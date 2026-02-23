#version 330 core
#include "include/JzShaderCommon.glsl"

layout (location = 0) in vec2 aPos;

JZ_VK_LAYOUT_LOCATION(0) out vec2 vUV;

void main()
{
    // Full-screen triangle in clockwise winding (compatible with current front-face culling setup)
    vUV = aPos * 0.5 + 0.5;

    // Keep depth close to far plane so skybox fills only untouched pixels after geometry pass.
    gl_Position = vec4(aPos.xy, 0.999999, 1.0);
    JZ_FLIP_CLIP_Y(gl_Position);
}
