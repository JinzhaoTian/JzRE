#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material structure with color support (no lighting)
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Uniforms
uniform Material material;

void main()
{
    // Simply output the material's diffuse color without any lighting calculations
    FragColor = vec4(material.diffuse, 1.0);
}
