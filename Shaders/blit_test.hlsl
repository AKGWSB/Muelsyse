// Ä¬ÈÏ²ÉÑùÆ÷
SamplerState DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PointSample(Texture2D tex, float2 uv)
{
    return tex.Sample(DefaultSampler, float2(uv.x, 1.0 - uv.y));
}

Texture2D mainTex0;
Texture2D mainTex1;

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
    //p = mul(p, modelMatrix);
    //p = mul(p, viewMatrix);
    //p = mul(p, projectionMatrix);

    result.position = p;
    result.texcoord = texcoord;
    result.normal = normal;
    result.tangent = tangent;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);

    //color.rg = input.texcoord;
    color.rgb += PointSample(mainTex0, input.texcoord);

    if (input.texcoord.x < 0.25 && input.texcoord.y < 0.25)
    {
        color.rgb = PointSample(mainTex1, input.texcoord * 4.0);
    }

    return color;
}
