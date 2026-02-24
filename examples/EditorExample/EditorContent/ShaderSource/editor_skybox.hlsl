struct VSInput
{
    float2 aPos : POSITION;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV       : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.UV = input.aPos * 0.5 + 0.5;
    output.Position = float4(input.aPos.xy, 0.999999, 1.0);
    return output;
}

cbuffer JzEditorSkyboxUniforms : register(b0, space0)
{
    float4x4 view;
    float4x4 projection;
    float3 topColor;
    float  _pad0;
    float3 horizonColor;
    float  _pad1;
    float3 groundColor;
    float  _pad2;
    float3 sunDirection;
    float  _pad3;
    float3 sunColor;
    float  sunSize;
    float  exposure;
    float3 _pad4;
};

float3 ComputeWorldDirection(float2 uv)
{
    const float2 ndc = uv * 2.0 - 1.0;
    const float3 worldDir = normalize(float3(ndc, 1.0));
    return worldDir;
}

float4 PSMain(VSOutput input) : SV_Target0
{
    const float3 dir = ComputeWorldDirection(input.UV);

    const float upFactor = saturate(dir.y * 0.5 + 0.5);

    float3 gradientColor = lerp(groundColor, horizonColor, smoothstep(0.0, 0.55, upFactor));
    gradientColor = lerp(gradientColor, topColor, smoothstep(0.45, 1.0, upFactor));

    const float3 nSunDir = normalize(-sunDirection);
    const float sunDot = max(dot(dir, nSunDir), 0.0);
    const float sunDisk = smoothstep(1.0 - max(sunSize, 0.0001), 1.0, sunDot);

    float3 color = gradientColor + sunColor * sunDisk * 1.5;
    color *= exposure;

    return float4(color, 1.0);
}
