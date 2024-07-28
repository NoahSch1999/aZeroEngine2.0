cbuffer DescriptorIndices : register(b0)
{
    int samplerIndex;
    int baseColorIndex;
    int worldNormalIndex;
    int worldPositionIndex;
};

struct FragmentInput
{
    float4 position : SV_Position;
    float2 uv : UV;
};

float4 main(FragmentInput input) : SV_TARGET
{
    SamplerState BasicSampler = SamplerDescriptorHeap[samplerIndex];
    
    Texture2D<float4> BaseColorTexture = ResourceDescriptorHeap[baseColorIndex];
    float4 BaseColor = float4(BaseColorTexture.Sample(BasicSampler, input.uv).xyz, 1.f);
    
    Texture2D<float4> WorldNormalTexture = ResourceDescriptorHeap[worldNormalIndex];
    float4 WorldNormal = float4(WorldNormalTexture.Sample(BasicSampler, input.uv).xyz, 1.f);
    
    if (
        WorldNormal.x < 0.0001f && WorldNormal.x > -0.0001f
        && WorldNormal.y < 0.0001f && WorldNormal.y > -0.0001f
        && WorldNormal.z < 0.0001f && WorldNormal.z > -0.0001f
        )
    {
        return BaseColor;
    }
    
    Texture2D<float4> WorldPositionTexture = ResourceDescriptorHeap[worldPositionIndex];
    float4 WorldPosition = float4(WorldPositionTexture.Sample(BasicSampler, input.uv).xyz, 1.f);
    
    float4 LightPos = float4(50, -100, -10, 1);
    float4 FragToLight = normalize(WorldPosition - LightPos);
    FragToLight.w = 0;
    float Angle = dot(FragToLight, WorldNormal);
    
    float ligth = 0.3f;
    if(Angle > 0.6)
    {
        ligth = 1.4f;
    }
    else if(Angle > 0.2 && Angle <= 0.6)
    {
        ligth = 0.9f;
    }
    
    float4 OutputColor = BaseColor * ligth;
    
    return OutputColor;
}