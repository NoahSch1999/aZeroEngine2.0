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
    float3 Position : POSITION;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct OutputData
{
    float4 Position : SV_Position;
    float2 UV : UV;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOSITION;
    float3x3 TBN : TBN;
};

OutputData main(VertexData Input)
{
    OutputData Output;
    Output.Position = float4(0, 0, 0, 1);
    //
    
    
	return Output;
}