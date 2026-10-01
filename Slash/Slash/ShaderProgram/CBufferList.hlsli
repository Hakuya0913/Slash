cbuffer TransformBuffer : register(b0)
{
    
    matrix world;
    matrix view;
    matrix proj;
    
}

cbuffer MaterialBuffer : register(b1)
{
    
    float4 baseColor;
    
    float meallic;
    float roughness;
    float ambientOcculusion;
    float padding;
    
    float3 emissiveColor;
    float emissiveStrength;
    
}