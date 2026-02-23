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
    vec3 materialAmbient;
    vec3 materialDiffuse;
    vec3 materialSpecular;
    float materialShininess;
    int hasDiffuseTexture;
} uStandardMaterial;
layout (set = 0, binding = 2) uniform sampler2D diffuseTexture;
#else
// Uniforms
uniform Material material;
uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;
#endif

void main()
{
    Material materialData;
    bool hasDiffuseTextureValue = false;
#if JZ_BACKEND_VULKAN
    materialData.ambient   = uStandardMaterial.materialAmbient;
    materialData.diffuse   = uStandardMaterial.materialDiffuse;
    materialData.specular  = uStandardMaterial.materialSpecular;
    materialData.shininess = uStandardMaterial.materialShininess;
    hasDiffuseTextureValue = (uStandardMaterial.hasDiffuseTexture != 0);
#else
    materialData           = material;
    hasDiffuseTextureValue = hasDiffuseTexture;
#endif

    vec3 finalColor;

    if (hasDiffuseTextureValue) {
        // Sample diffuse texture and multiply by material diffuse color
        vec4 texColor = texture(diffuseTexture, TexCoords);
        finalColor = texColor.rgb * materialData.diffuse;
    } else {
        // Use material diffuse color directly
        finalColor = materialData.diffuse;
    }

    FragColor = vec4(finalColor, 1.0);
}
