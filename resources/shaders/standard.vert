#version 330 core
#include "include/JzShaderCommon.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

JZ_VK_LAYOUT_LOCATION(0) out vec3 FragPos;
JZ_VK_LAYOUT_LOCATION(1) out vec3 Normal;
JZ_VK_LAYOUT_LOCATION(2) out vec2 TexCoords;

#if JZ_BACKEND_VULKAN
layout (set = 0, binding = 0) uniform JzStandardVertexUniforms {
    mat4 model;
    mat4 view;
    mat4 projection;
} uStandardVS;
#define model uStandardVS.model
#define view uStandardVS.view
#define projection uStandardVS.projection
#else
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
#endif

void main()
{
    // Transform position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal using the normal matrix (transpose of inverse of model matrix)
    // This correctly handles non-uniform scaling
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass through texture coordinates
    TexCoords = aTexCoords;
    
    // Final clip space position
    gl_Position = projection * view * vec4(FragPos, 1.0);
    JZ_FLIP_CLIP_Y(gl_Position);
}
