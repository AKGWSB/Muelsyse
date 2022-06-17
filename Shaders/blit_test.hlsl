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

float4 PSMain(PSInput input) : SV_TARGET
{
    input.texcoord.y = 1.0 - input.texcoord.y;

    float4 color = float4(0,0,0,1);
    color.rgb += mainTex.Sample(DefaultSampler, input.texcoord);

    /**/
    if (input.texcoord.x < 0.5f && input.texcoord.y < 0.5f)
    {
        float d = depthTex.Sample(DefaultSampler, input.texcoord * 2).r;
        color.rgb = float3(d, d, d);
    }

    return color;
}
