#version 330 core
#include "include/JzShaderCommon.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

JZ_VK_LAYOUT_LOCATION(0) out vec3 vColor;

#if JZ_BACKEND_VULKAN
layout (set = 0, binding = 0) uniform JzEditorAxisUniforms {
    mat4 model;
    mat4 view;
    mat4 projection;
} uEditorAxis;
#define model uEditorAxis.model
#define view uEditorAxis.view
#define projection uEditorAxis.projection
#else
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
#endif

void main()
{
    vColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    JZ_FLIP_CLIP_Y(gl_Position);
}
