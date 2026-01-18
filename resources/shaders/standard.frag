#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

// Material properties
uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;

// Light properties
uniform vec3 uLightDir;
uniform vec3 uLightColor;

// Camera position for specular
uniform vec3 uCameraPos;

void main()
{
    // Normalize the normal
    vec3 normal = normalize(vNormal);

    // Ambient
    vec3 ambient = uAmbientColor * 0.3;

    // Diffuse
    float diff = max(dot(normal, uLightDir), 0.0);
    vec3 diffuse = uDiffuseColor * diff * uLightColor;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    vec3 halfwayDir = normalize(uLightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uShininess);
    vec3 specular = uSpecularColor * spec * uLightColor * 0.5;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
