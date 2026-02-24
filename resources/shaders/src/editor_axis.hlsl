cbuffer JzEditorAxisUniforms : register(b0, space0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct VSInput
{
    float3 aPos   : POSITION;
    float3 aColor : COLOR0;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float3 Color    : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Color = input.aColor;
    output.Position = mul(projection, mul(view, mul(model, float4(input.aPos, 1.0))));
    return output;
}

float4 PSMain(VSOutput input) : SV_Target0
{
    return float4(input.Color, 1.0);
}
