// ========== UI.hlsl ==========

// 텍스처 및 샘플러
Texture2D gUITexture : register(t0);
SamplerState gSampler : register(s0);

// 상수 버퍼 (직교투영 행렬)
cbuffer CB : register(b0)
{
    float4x4 gWVP;
};

// Vertex Shader 입력
struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

// Vertex Shader 출력
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// UI Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), gWVP); // WVP 적용
    output.texcoord = input.texcoord;
    return output;
}

// UI Pixel Shader
float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color = gUITexture.Sample(gSampler, input.texcoord);  

    return color;
}
