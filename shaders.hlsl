

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj; // World-View-Projection 행렬
}

cbuffer cbMaterial : register(b1)
{

    float3 vAmbient;
    float padding1; // 4바이트 패딩
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
    int padding5; // 4바이트 패딩
}

cbuffer cbCamera : register(b2)
{
    float3 gCameraPos;
    float Lamp; //랜턴 유무 판별 0 없음 1 있음
    
    int l1;
    int l2;
    int l3;
    int l4;
    int l5;
    int l6;
    int l7;
    
    int padding;
}


// 텍스처와 샘플러
Texture2D gDiffuseTexture : register(t0); // 텍스처 자원
SamplerState gSampler : register(s0); // 샘플러 상태

struct VSInput
{
    float3 position : POSITION; // 정점 위치
    float3 normal : NORMAL; // 정점 법선
    float2 texcoord : TEXCOORD; // Texture coordinates
};

struct VSOutput
{
    float4 position : SV_POSITION; // 변환된 정점 위치
    float3 worldPos : TEXCOORD1; // 추가: 월드 좌표
    float3 normal : NORMAL; // 정점 법선
    float2 texcoord : TEXCOORD; // Texture coordinates
};

// Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = float4(input.position, 1.0f);///
    output.position = mul(float4(input.position, 1.0f), gWorldViewProj);
    output.worldPos = input.position; // 가정: input.position가 월드 좌표입니다.
    output.normal = normalize(input.normal);
    output.texcoord = input.texcoord; // 텍스처 좌표 전달
    return output;
}




// Pixel Shader
float4 PSMain(VSOutput input) : SV_TARGET
{ 
    
     // 정점 법선 정규화
    float3 normal = normalize(input.normal);
    
     // 텍스처 색상 결정 (bUseTexture가 true면 텍스처, 아니면 기본 색상)
    float3 texColor;
    float3 debugDiffuse = float3(0.35601, 0.38204, 0.49707); // 기본 색상 예시
    if (bUseTexture != 0)
    {
        texColor = gDiffuseTexture.Sample(gSampler, input.texcoord).rgb;
    }
    else
    {
        texColor = debugDiffuse;
    }
    
    // 카메라 위치(gCameraPos)와 픽셀의 월드 좌표(input.worldPos) 간의 벡터 계산
    float3 lightDir = normalize(input.worldPos - gCameraPos);
    float dist = length(input.worldPos - gCameraPos);
    // 거리 기반 감쇠 (range 값은 상황에 맞게 조절)
    float range = 20.0f; // 예시: 광원의 영향 범위
    if (Lamp == 0.0f)//랜턴 없을 경우
    {
        range = 10.0f;
    }
    else//랜턴 있을 경우 
    {
        range = 20.0f;
    }
    
    float attenuation = saturate(1.0f - (dist / range));

    // Lambert Diffuse 계산: 뒷면도 보이게 절대값 사용
    float NdotL = abs(dot(normal, lightDir));
    float3 diffuse1 = texColor * NdotL * attenuation;
    // 알파 값은 필요에 따라 조정 (예시에서는 1.0으로 고정)    
    float3 finalColor = diffuse1;
    
    // [2] 두 번째 점광원 (고정 위치 기반, 예: (1,1,1))
    if (l1 == 1)
    {
        float3 pointLightPos = float3(-18.211f, 0.0f, -29.418f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    if (l2 == 1)
    {
        float3 pointLightPos = float3(-6.17f, 0.0f, -24.132f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    if (l3 == 1)
    {
        float3 pointLightPos = float3(3.9131f, 0.0f, -24.132f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    if (l4 == 1)
    {
        float3 pointLightPos = float3(22.513f, 0.0f, -22.369f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    
    if (l5==1)
    {
        float3 pointLightPos = float3(-12.631f, 0.0f, -11.f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    if (l6 == 1)
    {
        float3 pointLightPos = float3(-1.55f, 0.0f, 2.7253f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    if (l7 == 1)
    {
        float3 pointLightPos = float3(-22.513f, 0.0f, -4.1611f); // 추가적인 점광원 위치
        float3 lightDir2 = normalize(input.worldPos - pointLightPos);
        float dist2 = length(input.worldPos - pointLightPos);
    
        float range2 = 15.0f; // 새로운 점광원의 범위 (적절히 조절 가능)
        float attenuation2 = saturate(1.0f - (dist2 / range2));

        float NdotL2 = abs(dot(normal, lightDir2));
        float3 diffuse2 = texColor * NdotL2 * attenuation2;

    // [3] 두 개의 점광원 조합
        finalColor += diffuse2;
    }
    return float4(finalColor, fAlpha);
   
  
}
