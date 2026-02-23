#version 330 core
#include "include/JzShaderCommon.glsl"

JZ_VK_LAYOUT_LOCATION(0) in vec3 vColor;

JZ_VK_LAYOUT_LOCATION(0) out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
