#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
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

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight light;

void main()
{
    // ambient
    vec3 ambient = texture(material.diffuse, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, light.direction), 0.0);
    vec3 diffuse = diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light.direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * texture(material.specular, TexCoords).rgb; 

	vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}