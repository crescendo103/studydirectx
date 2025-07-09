
// UI �ؽ�ó
Texture2D gUITexture : register(t0);
SamplerState gSampler : register(s0);

struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// UI Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f); // ���� ��ȯ ���� �״�� ���
    output.texcoord = input.texcoord;
    return output;
}

// UI Pixel Shader (�⺻������ ������ ���)
float4 PSMain(VSOutput input) : SV_TARGET
{
    
    float4 color = gUITexture.Sample(gSampler, input.texcoord);

    // Ÿ�� ������ (0.16, 0.02, 0.35)
    // �ణ�� ������ ����ϱ� ���� threshold ���� ����մϴ�.
    float threshold = 0.1;
    if (abs(color.r - 0.16) < threshold &&
        abs(color.g - 0.02) < threshold &&
        abs(color.b - 0.35) < threshold)
    {
        color.a = 0.0; // Ÿ�� ���� �ش��ϸ� ���� ���� ó��
    }

    return color;
}