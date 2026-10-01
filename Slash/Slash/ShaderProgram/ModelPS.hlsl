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
    
    return textureColor * baseColor;
    
}