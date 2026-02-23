#version 330 core
#include "include/JzShaderCommon.glsl"

JZ_VK_LAYOUT_LOCATION(0) in vec3 FragPos;
JZ_VK_LAYOUT_LOCATION(1) in vec3 Normal;
JZ_VK_LAYOUT_LOCATION(2) in vec2 TexCoords;

JZ_VK_LAYOUT_LOCATION(0) out vec4 FragColor;

// Material structure with full MTL properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

#if JZ_BACKEND_VULKAN
layout (set = 0, binding = 1) uniform JzStandardMaterialUniforms {
    Material material;
    int hasDiffuseTexture;
} uStandardMaterial;
#define material uStandardMaterial.material
#define hasDiffuseTexture uStandardMaterial.hasDiffuseTexture
layout (set = 0, binding = 2) uniform sampler2D diffuseTexture;
#else
// Uniforms
uniform Material material;
uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;
#endif

void main()
{
    vec3 finalColor;

    if (hasDiffuseTexture != 0) {
        // Sample diffuse texture and multiply by material diffuse color
        vec4 texColor = texture(diffuseTexture, TexCoords);
        finalColor = texColor.rgb * material.diffuse;
    } else {
        // Use material diffuse color directly
        finalColor = material.diffuse;
    }

    FragColor = vec4(finalColor, 1.0);
}
