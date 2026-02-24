cbuffer JzStandardVertexUniforms : register(b0, space0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct VSInput
{
    float3 aPos       : POSITION;
    float3 aNormal    : NORMAL;
    float2 aTexCoords : TEXCOORD0;
    float3 aTangent   : TANGENT;
    float3 aBitangent : BINORMAL;
};

struct VSOutput
{
    float4 Position  : SV_Position;
    float3 FragPos   : TEXCOORD0;
    float3 Normal    : TEXCOORD1;
    float2 TexCoords : TEXCOORD2;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    const float4 worldPos = mul(model, float4(input.aPos, 1.0));
    output.FragPos = worldPos.xyz;

    const float3x3 normalMatrix = (float3x3)model;
    output.Normal = normalize(mul(normalMatrix, input.aNormal));

    output.TexCoords = input.aTexCoords;
    output.Position = mul(projection, mul(view, worldPos));
    return output;
}

struct Material
{
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float  shininess;
};

cbuffer JzStandardMaterialUniforms : register(b1, space0)
{
    Material material;
    int      hasDiffuseTexture;
    float3   _padding;
};

Texture2D    diffuseTexture        : register(t2, space0);
SamplerState diffuseTextureSampler : register(s2, space0);

float4 PSMain(VSOutput input) : SV_Target0
{
    float3 finalColor = material.diffuse;

#if USE_DIFFUSE_MAP
    if (hasDiffuseTexture != 0)
    {
        const float4 texColor = diffuseTexture.Sample(diffuseTextureSampler, input.TexCoords);
        finalColor = texColor.rgb * material.diffuse;
    }
#endif

    return float4(finalColor, 1.0);
}
