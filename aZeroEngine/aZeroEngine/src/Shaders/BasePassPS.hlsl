cbuffer PixelShaderConstants : register(b0)
{
    int SamplerIndex;
};

struct FragmentInput
{
    float4 Position : SV_Position;
    float3 WorldPosition : WORLDPOSITION;
    float3 Normal : NORMAL;
    float2 UV : UV;
    float3x3 TBN : TBN;
};

struct FragmentOutput
{
    float4 FragmentColor : SV_TARGET0;
    float4 WorldPosition : SV_TARGET1;
    float4 WorldNormal : SV_TARGET2;
};

FragmentOutput main(FragmentInput input)
{
    FragmentOutput Output;
    SamplerState Sampler = SamplerDescriptorHeap[SamplerIndex];
    //Texture2D<float4> albedoTexture = ResourceDescriptorHeap[albedoDescriptorIndex];
    
    return Output;
}