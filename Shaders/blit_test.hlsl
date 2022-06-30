// Ä¬ÈÏ²ÉÑùÆ÷
SamplerState DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D mainTex;
Texture2D depthTex;

cbuffer cbPrePass
{
    float4 mainTex_texelSize;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
};

PSInput VSMain(
    float4 position : POSITION, 
    float2 texcoord : TEXCOORD, 
    float3 normal   : NORMAL, 
    float3 tangent  : TANGENT)
{
    PSInput result;

    float4 p = position;

    result.position = p;
    result.texcoord = texcoord;
    result.normal = normal;
    result.tangent = tangent;

    return result;
}

float3 ACESToneMapping(float3 color, float adapted_lum)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

float3 GammaCorrect(float3 color)
{
    // gamma
    float g = 1.0 / 2.2;
    color.rgb = saturate(pow(color.rgb, float3(g, g, g)));
    return color;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    input.texcoord.y = 1.0 - input.texcoord.y;

    float4 color = float4(0,0,0,1);
    color.rgb += mainTex.Sample(DefaultSampler, input.texcoord);

    // tone map
    //color.rgb = ACESToneMapping(color.rgb, 1.0);

    // gamma
    //color.rgb = GammaCorrect(color.rgb);

    return color;
}
