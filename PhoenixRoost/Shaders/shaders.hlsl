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
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
