/*
struct AppData
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 texcoord : TEXCOORD;
};

struct v2f
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 texcoord : TEXCOORD;
};

v2f VSMain(AppData i)
{
    v2f o;
    o.position = i.position;
    o.normal = i.normal;
    o.tangent = i.tangent;
    o.texcoord = i.texcoord;

    return o;
}

float4 PSMain(v2f i) : SV_TARGET
{
    return float4(1,0,0,1);
}
*/

// ----------------

/*
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

PSInput VSMain(float4 position : POSITION, float4 texcoord : TEXCOORD)
{
    PSInput result;

    result.position = position;
    result.texcoord = texcoord;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.texcoord;
    //return g_texture.Sample(g_sampler, texcoord);
    //return float4(texcoord.x, texcoord.y, 0, 1);
}*/

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = position;
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return float4(input.uv.x, input.uv.y, 0, 1);
    return g_texture.Sample(g_sampler, input.uv);
}
