#include"CBufferList.hlsli"

struct VSInput
{
    
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    
};

struct VSOutput
{
    
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    
};


VSOutput main(VSInput input)
{
    
    VSOutput output;
    
    float4 position = float4(input.position, 1.0f);
    
    position = mul(position, world);
    position = mul(position, view);
    position = mul(position, proj);
    
    output.position = position;
    output.texCoord = input.texCoord;
    
    return output;
    
}