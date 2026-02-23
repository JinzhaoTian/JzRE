#version 330 core
#include "include/JzShaderCommon.glsl"

JZ_VK_LAYOUT_LOCATION(0) in vec2 vUV;

JZ_VK_LAYOUT_LOCATION(0) out vec4 FragColor;

#if JZ_BACKEND_VULKAN
layout (set = 0, binding = 0) uniform JzEditorSkyboxUniforms {
    mat4 view;
    mat4 projection;
    vec3 topColor;
    vec3 horizonColor;
    vec3 groundColor;
    vec3 sunDirection;
    vec3 sunColor;
    float sunSize;
    float exposure;
} uSkybox;
#define view uSkybox.view
#define projection uSkybox.projection
#define topColor uSkybox.topColor
#define horizonColor uSkybox.horizonColor
#define groundColor uSkybox.groundColor
#define sunDirection uSkybox.sunDirection
#define sunColor uSkybox.sunColor
#define sunSize uSkybox.sunSize
#define exposure uSkybox.exposure
#else
uniform mat4 view;
uniform mat4 projection;

uniform vec3 topColor;
uniform vec3 horizonColor;
uniform vec3 groundColor;
uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform float sunSize;
uniform float exposure;
#endif

vec3 ComputeWorldDirection(vec2 uv)
{
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 clipPos = vec4(ndc, 1.0, 1.0);
    vec4 viewPos = inverse(projection) * clipPos;

    // Treat as direction vector, not position.
    vec4 viewDir = vec4(viewPos.xy, -1.0, 0.0);
    vec3 worldDir = normalize((inverse(view) * viewDir).xyz);
    return worldDir;
}

void main()
{
    vec3 dir = ComputeWorldDirection(vUV);

    float upFactor = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);

    vec3 gradientColor = mix(groundColor, horizonColor, smoothstep(0.0, 0.55, upFactor));
    gradientColor = mix(gradientColor, topColor, smoothstep(0.45, 1.0, upFactor));

    vec3 nSunDir = normalize(-sunDirection);
    float sunDot = max(dot(dir, nSunDir), 0.0);
    float sunDisk = smoothstep(1.0 - max(sunSize, 0.0001), 1.0, sunDot);

    vec3 color = gradientColor + sunColor * sunDisk * 1.5;
    color *= exposure;

    FragColor = vec4(color, 1.0);
}
