cbuffer VertexShaderConstants : register(b0)
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
};

cbuffer PrimitiveData : register(b1)
{
    int WorldSpaceMatrix;
};

struct VertexData
{
    float3 Position;
    float2 UV;
    float3 Normal;
    float3 Tangent;
};

struct OutputData
{
    float4 Position : SV_Position;
    float3 WorldPosition : WORLDPOSITION;
    float3 Normal : NORMAL;
    float2 UV : UV;
    float3x3 TBN : TBN;
};

OutputData main(VertexData Input)
{
    OutputData Output;
    
    //
    
    
	return Output;
}