cbuffer cbMVPObject
{
    float4x4 WVP;
};

struct VertexInput
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.pos = mul(input.pos, WVP);

    output.col = input.col;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return input.col;
}
