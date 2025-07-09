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

    // �Է� ��ġ�� �ؽ�ó ��ǥ�� �ѱ� (����ȭ�� ���� ����)
    output.UV = input.UV;
     // ��ġ�� ViewProj�� ��ȯ
    float4 pos = float4(input.Pos, 1.0f);
    output.Pos = mul(SkyViewProj, pos); // ���� ����: column-major
    output.Pos.z = output.Pos.w;
    
   
    return output;
}


float4 PS_Skybox(PSInput input) : SV_TARGET
{
    // ť��ʿ��� ���⺤�ͷ� �� ���ø�
   
    return skyboxTexture.Sample(skySampler, input.UV);
}
