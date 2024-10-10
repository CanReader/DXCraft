SamplerState ObjSamplerState;
TextureCube SkyMap;

cbuffer cbMVPObject
{
    float4x4 WVP;

};  

struct SKYMAP_VS_OUTPUT	
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 texCoord : TEXCOORD;
};

SKYMAP_VS_OUTPUT VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT) 0;

    output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;
    output.Normal = normal;
    output.texCoord = inPos;

    return output;
}

float4 PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.texCoord);
}
