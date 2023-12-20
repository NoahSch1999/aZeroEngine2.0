cbuffer MaterialConstants : register(b0)
{
    int albedoDescriptorIndex;
    int normalDescriptorIndex;
};

cbuffer SamplerSpecs : register(b1)
{
    int samplerIndex;
};

struct FragmentInput
{
    float4 position : SV_Position;
    float3 worldPosition : WORLDPOSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3x3 TBN : TBN;
};

struct FragmentOutput
{
    float4 albedoColor : SV_TARGET0;
    //float4 normalData : SV_TARGET1;
};

FragmentOutput main(FragmentInput input)
{
    SamplerState basicSampler = SamplerDescriptorHeap[samplerIndex];
    
    float4 color = float4(1, 1, 1, 1);
    float3 fragmentNormal = input.normal;
    
    if(albedoDescriptorIndex != -1)
    {
        Texture2D<float4> albedoTexture = ResourceDescriptorHeap[albedoDescriptorIndex];
        color = float4(albedoTexture.Sample(basicSampler, input.uv).xyz, 1.f);
    }
    
    if (normalDescriptorIndex != -1)
    {
        Texture2D<float4> normalTexture = ResourceDescriptorHeap[normalDescriptorIndex]; // TODO - Change to float3
        fragmentNormal = normalTexture.Sample(basicSampler, input.uv).xyz;
        fragmentNormal = normalize(fragmentNormal * 2.0 - 1.0);
        fragmentNormal = normalize(mul(fragmentNormal, input.TBN));
    }
    
    FragmentOutput Output;
    Output.albedoColor = color;
    //Output.normalData = float4(fragmentNormal, 0.f);
    
    return Output;
}