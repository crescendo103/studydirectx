cbuffer SkyboxConstants : register(b0)
{
    float4x4 SkyViewProj;
};

Texture2D skyboxTexture : register(t0);
SamplerState skySampler : register(s0);


struct VSInput
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD0;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

PSInput VS_Skybox(VSInput input)
{
    PSInput output;

    // 입력 위치를 텍스처 좌표로 넘김 (정규화된 방향 벡터)
    output.UV = input.UV;
     // 위치는 ViewProj로 변환
    float4 pos = float4(input.Pos, 1.0f);
    output.Pos = mul(SkyViewProj, pos); // 순서 주의: column-major
    output.Pos.z = output.Pos.w;
    
   
    return output;
}


float4 PS_Skybox(PSInput input) : SV_TARGET
{
    // 큐브맵에서 방향벡터로 색 샘플링
   
    return skyboxTexture.Sample(skySampler, input.UV);
}
