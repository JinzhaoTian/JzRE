#version 330 core

#define MAX_TEXTURES 9

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D diffuse[MAX_TEXTURES];
    sampler2D specular[MAX_TEXTURES];
    sampler2D normal[MAX_TEXTURES];
    sampler2D height[MAX_TEXTURES];
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

uniform int numDiffuseTextures;
uniform int numSpecularTextures;
uniform int numNormalTextures;
uniform int numHeightTextures;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight[1];

void main()
{
    // ambient
    float ambientStrength = 1.0;
    vec3 ambientColor = vec3(0.0);
    for (int i = 0; i < numDiffuseTextures; i++) {
        ambientColor += ambientStrength * texture(material.diffuse[i], TexCoords).rgb;
    }
    vec3 ambient = directionalLight[0].color * ambientColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, directionalLight[0].direction), 0.0);
    vec3 diffuseColor = vec3(0.0);
    for (int i = 0; i < numDiffuseTextures; i++) {
        diffuseColor += diff * texture(material.diffuse[i], TexCoords).rgb;
    }
    vec3 diffuse = directionalLight[0].color * diffuseColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-directionalLight[0].direction, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = vec3(0.0);
    for (int i = 0; i < numSpecularTextures; i++) {
        specularColor += spec * texture(material.specular[i], TexCoords).rgb;
    }
    vec3 specular = directionalLight[0].color * specularColor; 

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}