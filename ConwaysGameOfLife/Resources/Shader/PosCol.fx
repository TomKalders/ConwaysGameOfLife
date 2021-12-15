//-----------------------------
//	Variables
//-----------------------------
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gInverseViewMatrix : VIEWINVERSE;
int gFilterMethod : FILTER;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577);
float gLightIntesity = float(1.f);

//-----------------------------
//	Rasterizer states
//-----------------------------
RasterizerState gRasterizerState
{
	CullMode = back;
	FrontCounterClockWise = true;
};

//-----------------------------
//	Blend states
//-----------------------------
BlendState gBlendState
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = false;
    SrcBlend = one;
    DestBlend = zero;
    BlendOp = add;
    SrcBlendAlpha = one;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

//-----------------------------
//	DepthStencil states
//-----------------------------
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
    StencilEnable = false;

    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;

    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

//-----------------------------
//	Input/Output structs
//-----------------------------
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

VS_OUTPUT VS(VS_INPUT input)
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

float4 CalculateLambert(float3 normal)
{   
    //float4 lambert = float4(1, 1, 1, 1) * (gLightIntesity * (dot(-normal, gLightDirection)));
    float4 lambert = float4(1, 1, 1, 1) * (gLightIntesity * dot(-normal, gLightDirection));
    return saturate(lambert);
}

//-----------------------------
//	Pixel Shader
//-----------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //const float3 binormal = cross(input.Tangent, input.Normal);
    //float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);
    //tangentSpaceAxis = transpose(tangentSpaceAxis);
	
    //float3 normalSample = input.Normal;
    //normalSample = (2 * normalSample) - (1, 1, 1);
    //const float3 normal = normalize(mul(tangentSpaceAxis, normalSample));
    
    //return saturate((float4(input.Color, 1)) / CalculateLambert(input.Normal));
    return (float4(0.3f, 0.3f, 0.3f, 1)) / CalculateLambert(input.Normal);
    //float power = input.Power / 255.0f;
    //return float4(power, power, power, 1);
}

//-----------------------------
//	Technique
//-----------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
        //SetDepthStencilState(gDepthStencilState, 0);
        //SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}