#version 330 core
#include "include/JzShaderCommon.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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
