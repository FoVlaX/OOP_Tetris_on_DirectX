Texture2D txDiffuse : register(t0);        
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

cbuffer ConstantBufferPointLight : register ( b2 )
{
	float4 vPos;
	float4 vLightPointColor;
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
	float4 PosX : POSITION;
};


PS_INPUT VS( VS_INPUT input) 
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul( input.Pos, World );
	output.PosX = output.Pos;
	output.Pos = mul( output.Pos, View);
	output.Pos = mul( output.Pos, Projection);
	output.Norm = mul( input.Norm, World);
	output.Tex = input.Tex;
	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	float4 finalcolor = 0.1f;
	/*for (int i = 0; i < 1; i++)
	{
		finalcolor += saturate(dot((float3)vLightDir[i], input.Norm) * vLightColor[i]);
	}*/
	float R = vPos.a;
	


	float3 vLightPointDir = (float3)input.PosX - (float3)vPos;
	float k = length(vLightPointDir);
	
	if (k > R) k = R;
	k = 1 -  k / R;
	
	finalcolor += saturate(dot(vLightPointDir, input.Norm) * k * vLightPointColor);

	finalcolor *= txDiffuse.Sample(samLinear, input.Tex);
	finalcolor.a = 1.f;
	return finalcolor;
}