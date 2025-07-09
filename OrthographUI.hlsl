// ========== UI.hlsl ==========

// �ؽ�ó �� ���÷�
Texture2D gUITexture : register(t0);
SamplerState gSampler : register(s0);

// ��� ���� (�������� ���)
cbuffer CB : register(b0)
{
    float4x4 gWVP;
};

// Vertex Shader �Է�
struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

// Vertex Shader ���
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// UI Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), gWVP); // WVP ����
    output.texcoord = input.texcoord;
    return output;
}

// UI Pixel Shader
float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 color = gUITexture.Sample(gSampler, input.texcoord);  

    return color;
}
