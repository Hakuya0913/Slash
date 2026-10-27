#include"CBufferList.hlsli"

struct VSInput
{
    
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    
    uint4 boneIndices : BONEINDICES;
    uint4 boneWeights : BONEWEIGHTS;
    
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
    
    float totalWeight =
        input.boneWeights.x +
        input.boneWeights.y +
        input.boneWeights.z +
        input.boneIndices.w;
    
    if(totalWeight > 0.0f)
    {
        
        float4 skinnedPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        skinnedPosition += mul(position, boneMatrices[input.boneIndices.x]) * input.boneWeights.x;
        skinnedPosition += mul(position, boneMatrices[input.boneIndices.y]) * input.boneWeights.y;
        skinnedPosition += mul(position, boneMatrices[input.boneIndices.z]) * input.boneWeights.z;
        skinnedPosition += mul(position, boneMatrices[input.boneIndices.w]) * input.boneWeights.w;
        
        position = skinnedPosition;
        
    }
    
    position = mul(position, world);
    position = mul(position, view);
    position = mul(position, proj);
    
    output.position = position;
    output.texCoord = input.texCoord;
    
    return output;
    
}