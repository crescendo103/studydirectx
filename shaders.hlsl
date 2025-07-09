

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj; // World-View-Projection ���
}

cbuffer cbMaterial : register(b1)
{

    float3 vAmbient;
    float padding1; // 4����Ʈ �е�
    float3 vDiffuse;
    float padding2;
    float3 vSpecular;
    float padding3;
    float3 vEmissive;
    float padding4;
    
    uint nShininess;
    float fAlpha;
    int bSpecular;
    int bEmissive;
    
    int bUseTexture;
    int padding5; // 4����Ʈ �е�
}

cbuffer cbCamera : register(b2)
{
    float3 gCameraPos;
    float Lamp; //���� ���� �Ǻ� 0 ���� 1 ����
    
    int l1;
    int l2;
    int l3;
    int l4;
    int l5;
    int l6;
    int l7;
    
    int padding;
}


// �ؽ�ó�� ���÷�
Texture2D gDiffuseTexture : register(t0); // �ؽ�ó �ڿ�
SamplerState gSampler : register(s0); // ���÷� ����

struct VSInput
{
    float3 position : POSITION; // ���� ��ġ
    float3 normal : NORMAL; // ���� ����
    float2 texcoord : TEXCOORD; // Texture coordinates
};

struct VSOutput
{
    float4 position : SV_POSITION; // ��ȯ�� ���� ��ġ
    float3 worldPos : TEXCOORD1; // �߰�: ���� ��ǥ
    float3 normal : NORMAL; // ���� ����
    float2 texcoord : TEXCOORD; // Texture coordinates
};

// Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = float4(input.position, 1.0f);///
    output.position = mul(float4(input.position, 1.0f), gWorldViewProj);
    output.worldPos = input.position; // ����: input.position�� ���� ��ǥ�Դϴ�.
    output.normal = normalize(input.normal);
    output.texcoord = input.texcoord; // �ؽ�ó ��ǥ ����
    return output;
}




// Pixel Shader
float4 PSMain(VSOutput input) : SV_TARGET
{ 
    
     // ���� ���� ����ȭ
    float3 normal = normalize(input.normal);
    
     // �ؽ�ó ���� ���� (bUseTexture�� true�� �ؽ�ó, �ƴϸ� �⺻ ����)
    float3 texColor;
    float3 debugDiffuse = float3(0.35601, 0.38204, 0.49707); // �⺻ ���� ����
    if (bUseTexture != 0)
    {
        texColor = gDiffuseTexture.Sample(gSampler, input.texcoord).rgb;
    }
    else
    {
        texColor = debugDiffuse;
    }
    
    // ī�޶� ��ġ(gCameraPos)�� �ȼ��� ���� ��ǥ(input.worldPos) ���� ���� ���
    float3 lightDir = normalize(input.worldPos - gCameraPos);
    float dist = length(input.worldPos - gCameraPos);
    // �Ÿ� ��� ���� (range ���� ��Ȳ�� �°� ����)
    float range = 20.0f; // ����: ������ ���� ����
    if (Lamp == 0.0f)//���� ���� ���
    {
        range = 10.0f;
    }
    else//���� ���� ��� 
    {
        range = 20.0f;
    }
    
    float attenuation = saturate(1.0f - (dist / range));

    // Lambert Diffuse ���: �޸鵵 ���̰� ���밪 ���
    float NdotL = abs(dot(normal, lightDir));
    float3 diffuse1 = texColor * NdotL * attenuation;
    // ���� ���� �ʿ信 ���� ���� (���ÿ����� 1.0���� ����)    
    float3 finalColor = diffuse1;
    
    // [2] �� ��° ������ (���� ��ġ ���, ��: (1,1,1))
    if (l1 == 1)
    {
        float3 pointLightPos = float3(-18.211f, 0.0f, -29.418f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    if (l2 == 1)
    {
        float3 pointLightPos = float3(-6.17f, 0.0f, -24.132f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    if (l3 == 1)
    {
        float3 pointLightPos = float3(3.9131f, 0.0f, -24.132f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    if (l4 == 1)
    {
        float3 pointLightPos = float3(22.513f, 0.0f, -22.369f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    
    if (l5==1)
    {
        float3 pointLightPos = float3(-12.631f, 0.0f, -11.f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    if (l6 == 1)
    {
        float3 pointLightPos = float3(-1.55f, 0.0f, 2.7253f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    if (l7 == 1)
    {
        float3 pointLightPos = float3(-22.513f, 0.0f, -4.1611f); // �߰����� ������ ��ġ
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // ���ο� �������� ���� (������ ���� ����)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] �� ���� ������ ����
        finalColor += diffuse2;
    }
    return float4(finalColor, fAlpha);
   
  
}
