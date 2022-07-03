// Ä¬ÈÏ²ÉÑùÆ÷
SamplerState DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbPreObject
{
    float4x4 modelMatrix;
};

cbuffer cbPrePass
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
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
    p = mul(p, modelMatrix);
    p = mul(p, viewMatrix);
    p = mul(p, projectionMatrix);

    result.position = p;
    result.texcoord = texcoord;
    result.normal = normal;
    result.tangent = tangent;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);
    color.rgb += float3(1,1,1);

    float3 N = input.normal;
    float3 L = normalize(float3(1, 1, 0));
    float NdotL = saturate(dot(N, L));
    color.rgb *= NdotL;

    return color;
}
