#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
using namespace DirectX;
using namespace Microsoft::WRL;



struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;
};


struct UIVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 textureCoordinate;
};

struct Subset {
    ComPtr<ID3D12Resource> materialBuffer = nullptr; // 기본값으로 nullptr 설정
    D3D12_GPU_VIRTUAL_ADDRESS materialBufferAddress = 0; // GPU 주소는 기본값 0으로 설정
    int offset = 0; // 기본값 0
    bool istexture = false;
};

struct Mainset {
    D3D12_INDEX_BUFFER_VIEW indicesBufferView;
    ComPtr<ID3D12Resource> indicesBuffer;
    UINT indicesCount;
    DirectX::BoundingBox Box;
};

struct CamWorld {
    DirectX::XMFLOAT3 gCameraPos;
    float Lamp;
    //int outsideLamp[7];
    
    int l1;
    int l2;
    int l3;
    int l4;
    int l5;
    int l6;
    int l7;
    
    int padding;
};

struct alignas(256) CBData
{
    DirectX::XMMATRIX gWVP;
};

struct SkyboxConstants
{
    XMMATRIX SkyViewProj; 
};

struct SkyVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 textureCoordinate;
};

struct Vec3 {
    float x;
    float y;
    float z;
};

struct MinMax {
    float maxX = -1000.f;
    float maxZ = -1000.f;
    float minX = 1000.f;
    float minZ = 1000.f;
    float anyYpos = 0.0f;
};

