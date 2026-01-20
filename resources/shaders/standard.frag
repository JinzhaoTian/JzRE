#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material structure with full MTL properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Uniforms
uniform Material material;
uniform sampler2D diffuseTexture;
uniform bool hasDiffuseTexture;

void main()
{
    vec3 finalColor;

    if (hasDiffuseTexture) {
        // Sample diffuse texture and multiply by material diffuse color
        vec4 texColor = texture(diffuseTexture, TexCoords);
        finalColor = texColor.rgb * material.diffuse;
    } else {
        // Use material diffuse color directly
        finalColor = material.diffuse;
    }

    FragColor = vec4(finalColor, 1.0);
}
