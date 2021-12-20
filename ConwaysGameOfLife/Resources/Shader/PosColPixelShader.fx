struct PS_INPUT
{
	float4 Position : SV_POSITION;
    float3 Color : COLOR;
	float3 WorldPosition : WORLDPOSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float Power : POWER;
};

struct PS_OUTPUT
{
    float4 RGBColor : SV_TARGET;  // pixel color (your PS computes this system value)
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT Output;

    float power = input.Power / 255.0f;
    Output.RGBColor = float4(power, power, power, 1);

    return Output;
}