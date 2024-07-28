cbuffer ComputeShaderConstants : register(b0)
{
    int OutputTargetUAVIndex;
    int FragmentColorGBufferSRVIndex;
    int WorldPositionGBufferSRVIndex;
    int NormalGBufferSRVIndex;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Texture2D<float4> FragmentColorTexture = ResourceDescriptorHeap[FragmentColorGBufferSRVIndex];
    Texture2D<float4> WorldPositionTexture = ResourceDescriptorHeap[WorldPositionGBufferSRVIndex];
    Texture2D<float4> NormalTexture = ResourceDescriptorHeap[NormalGBufferSRVIndex];
    
    float4 FragmentColor = FragmentColorTexture.Load(float3(DTid.x, DTid.y, 0));
    float4 WorldPosition = float4(WorldPositionTexture.Load(float3(DTid.x, DTid.y, 0)).xyz, 1.f);
    float4 Normal = float4(NormalTexture.Load(float3(DTid.x, DTid.y, 0)).xyz, 0.f);
    
}