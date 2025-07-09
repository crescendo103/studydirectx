
// UI 텍스처
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
    output.position = float4(input.position, 1.0f); // 월드 변환 없이 그대로 사용
    output.texcoord = input.texcoord;
    return output;
}

// UI Pixel Shader (기본적으로 빨간색 출력)
float4 PSMain(VSOutput input) : SV_TARGET
{
    
    float4 color = gUITexture.Sample(gSampler, input.texcoord);

    // 타겟 색상은 (0.16, 0.02, 0.35)
    // 약간의 오차를 허용하기 위해 threshold 값을 사용합니다.
    float threshold = 0.1;
    if (abs(color.r - 0.16) < threshold &&
        abs(color.g - 0.02) < threshold &&
        abs(color.b - 0.35) < threshold)
    {
        color.a = 0.0; // 타겟 색상에 해당하면 완전 투명 처리
    }

    return color;
}