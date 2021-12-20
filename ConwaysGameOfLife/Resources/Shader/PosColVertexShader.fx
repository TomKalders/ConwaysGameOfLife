cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    float4x4 gWorldViewProj : WorldViewProjection;
    float4x4 gWorldMatrix : WORLD;
};

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
    float Power : POWER;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float3 Color : COLOR;
	float3 WorldPosition : WORLDPOSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float Power : POWER;
};

VS_OUTPUT main(VS_INPUT input) // main is the default function name
{
	VS_OUTPUT output = (VS_OUTPUT)0;
    //output.Position = float4(input.Position, 1);
	output.Position = mul(gWorldViewProj, float4(input.Position, 1.f));
    output.WorldPosition = mul(input.Position, (float3x3)gWorldMatrix);
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
    output.Color = input.Color;
    output.Power = input.Power;
    return output;
}