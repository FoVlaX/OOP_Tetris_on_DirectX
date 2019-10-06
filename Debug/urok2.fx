Texture2D txDiffuse : register(t0);        
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer ConstantBufferLight : register( b1 )
{
	float4 vLightDir[2];
	float4 vLightColor[2];
	float4 vOutputColor;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : TEXCOORD1;
};


PS_INPUT VS( VS_INPUT input) 
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul( input.Pos, World );
	output.Pos = mul( output.Pos, View);
	output.Pos = mul( output.Pos, Projection);
	output.Norm = mul( input.Norm, World);
	output.Tex = input.Tex;
	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalcolor = 0;
	for (int i = 0; i < 1; i++)
	{
		finalcolor += saturate(dot((float3)vLightDir[i], input.Norm) * vLightColor[i]);
	}
	finalcolor *= txDiffuse.Sample(samLinear, input.Tex);
	finalcolor.a = 1.f;
	return finalcolor;
}