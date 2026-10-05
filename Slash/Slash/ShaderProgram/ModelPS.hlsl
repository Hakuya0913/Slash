#include"CBufferList.hlsli"

struct PSInput
{
    
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
    
};

Texture2D DiffuseTexture : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    
    float4 textureColor = DiffuseTexture.Sample(
        LinearSampler,
        input.texCoord
       );
    
    //試験用
    return float4(baseColor.r, 0.0f, 0.0f, 1.0f);
    
    return textureColor * baseColor;
    
}