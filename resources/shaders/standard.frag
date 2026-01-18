#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material structure with color support (instead of textures)
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Directional light structure (matching example shader)
struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

// Uniforms
uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directionalLight[1];

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Light direction (already normalized from CPU)
    vec3 lightDir = normalize(directionalLight[0].direction);
    
    // ==================== Ambient ====================
    // Base ambient lighting using material's diffuse color for better color representation
    vec3 ambient = directionalLight[0].color * material.diffuse * 0.3;
    
    // ==================== Diffuse ====================
    // Lambertian diffuse - calculate how directly the surface faces the light
    float NdotL = dot(norm, lightDir);
    
    // Support both front and back faces for enclosed scenes (like Cornell Box)
    float diff = max(NdotL, 0.0);
    float backDiff = max(-NdotL, 0.0) * 0.2;  // Back faces get reduced lighting
    float totalDiff = diff + backDiff;
    
    vec3 diffuse = directionalLight[0].color * material.diffuse * totalDiff;
    
    // ==================== Specular ====================
    // Blinn-Phong specular - only calculate for front-facing surfaces
    vec3 specular = vec3(0.0);
    if (NdotL > 0.0) {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = directionalLight[0].color * material.specular * spec * 0.5;
    }
    
    // ==================== Final Color ====================
    vec3 result = ambient + diffuse + specular;
    
    // Clamp to valid color range
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}
