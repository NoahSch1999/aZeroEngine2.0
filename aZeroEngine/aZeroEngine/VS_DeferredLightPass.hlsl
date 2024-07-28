struct VertexIn
{
    float2 position : POSITION;
    float2 uv : UV;
};

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv : UV;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    output.position = float4(input.position, 0, 1);
    output.uv = input.uv;
    return output;
}