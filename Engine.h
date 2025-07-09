#pragma once

#include <windows.h>
#include <comdef.h>
#include <fstream>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include <mmsystem.h>
#include <conio.h>
#include "DXSample.h"
#include "typedef.h"
#include "WaveFrontReader.h"
#include "Camera.h"
#include "RenderManager.h"
#include "thread"
#include "PathFinder.h"
#include <functional>  // std::reference_wrapper
#include "GemUI.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DX;
using index_t = uint16_t;

class Engine : public DXSample
{
public:
    Engine(UINT width, UINT height, std::wstring name, HWND hwnd);

	void OnInit();
	void LoadPipeline();
	void LoadAssets();
	void OnUpdate(float deltaX, float deltaY);
	void OnRender();
	void PopulateCommandList();
	void WaitForPreviousFrame();
	void OnDestroy();
    void BuildCameraConstantBuffer();
    UINT CalcConstantBufferByteSize(UINT byteSize);
    void CopyData(int elementIndex, Camera::ObjectConstants& data);
    void CreateSampler();
   
    void Engine::ReleaseResources();
    void CreateDepthBuffer();
    void RenderUI();
    
    void CreateUIVertexBuffer();
    void CreateUIRootSignature();
    void CreateUIPipelineState();
    
    void CreateEmptyDescriptor();

    void Pick(float x, float y);
    void CameraworldBuffer();

    void RenderLamp();
    void RenderRockCount();
    bool isLookObject(const XMFLOAT3& objectPos);

    void createOrthographSquare();
    void createOrthographRootsignature();
    void renderStartUI();
    void createOrthographConstantBuffer();
    void createOrthographMatrix();
    void CreateOrthographPipelineState();

    bool getshouldMouseCenter();
    
    void createSkyBoxConstantBuffer();
    void createSkyBoxPipelineState();
    void createSkyBoxRootsignature();
    void skyboxRender();
    void createSkyBox();

private:
    static const UINT FrameCount = 2;

    HWND m_hwnd;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12Resource> constantBuffer;
    UINT m_rtvDescriptorSize;
    ComPtr<ID3D12Resource> mUploadBuffer;
    ComPtr<ID3D12DescriptorHeap> srvHeap;
    //something wrong
    ComPtr<ID3D12DescriptorHeap> mCbvSrvHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle;
    //CD3DX12_CPU_DESCRIPTOR_HANDLE cbv2Handle;
    UINT mCbvSrvDescriptorSize;

    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW indicesBufferView;
    ComPtr<ID3D12Resource> indicesBuffer;

    ComPtr<ID3D12Resource> constantUploadBuffer;
    ComPtr<ID3D12Resource> constantUploadBufferUI;

    BYTE* mMappedData = nullptr;
    BYTE* mMappedDataUI = nullptr;
    BYTE* mMappedDataW = nullptr;
    Camera* cam;
    fps* fpsInstance; // fps 객체 저장
    /*
    ComPtr<ID3D12Resource> materialBuffer;
    UINT materialBufferSize;
    D3D12_GPU_VIRTUAL_ADDRESS materialBufferAddress;*/
    UINT materialBufferSize;

    std::unordered_map<uint32_t, std::vector<index_t>> materialToIndices;
    std::vector<Subset> leaf;
    std::vector <Mainset> tree;
    std::vector<RenderManager> master;
    std::vector<RenderManager> master2;
    std::vector<RenderManager> PickTable;
    std::vector<RenderManager> PickTableLamp;
    std::vector<bool> isLampOn;
    std::vector< RenderManager> keyAndDoor;

    ComPtr<ID3D12DescriptorHeap> mSamplerDescriptorHeap;

    std::vector<RenderManager> pacman;


    int facmanNum;
    int frameCount = 0;  // 프레임 카운터 변수
    const int frameDelay = 3;  // 몇 프레임마다 변경할지 설정
    int pathCount = 0;
    const int pathDelay = 5000;
    std::vector<RenderManager> key;

    int cc = 0;
    const int ccdelay = 100;

    bool iskeyend = false;
    bool isinitcomplete = false;


    std::vector<HBITMAP> hBitmapss;
    RECT clientRectt;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

    float width;
    float height;
    ComPtr<ID3D12PipelineState> m_uiPipelineState;

    ComPtr<ID3D12Resource> m_uiVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_uiVertexBufferView;
    ComPtr<ID3D12RootSignature> m_uiRootSignature;
    int UIoffset;

    D3D12_GPU_DESCRIPTOR_HANDLE m_nullSrvGpuDescriptorHandle;
    Transform* janitor;
    int janitorOffset;
    int masterCount;

    bool wasMousePressed = false;
    XMFLOAT2 whatGem = { 0.0f, 0.0f };
    bool shouldUI = false;

    ComPtr<ID3D12Resource> CameraWorldBuffer = nullptr; // 기본값으로 nullptr 설정
    D3D12_GPU_VIRTUAL_ADDRESS CameraWorldBufferAddress = 0;
    std::vector<RenderManager> Lamp;

    int Lampstart;
    bool ishaveLamp = false;
    
    std::vector<bool> keyHave;
    int Countstart;
    std::vector<RenderManager> gemCount;
    int compactGemMasterOffset;
    
    std::vector<RenderManager> compactGem;
    std::vector<RenderManager> rockTable;

    ComPtr<ID3D12Resource> m_OrthographVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_OrthographVertexBufferView;
    ComPtr<ID3D12RootSignature> m_OrthographRootSignature;
    ComPtr<ID3D12Resource> m_cbWVP;
    ComPtr<ID3D12PipelineState> m_OrthographPipelineState;

    int non;
    int gameStart;
    int gameEnd;

    bool isMenu = true;
    bool cursorOnStart;
    bool cursorOnEnd;

    bool shouldMouseCenter=false;
    bool pastEsc = false;

    int skyBox;
    //XMMATRIX wvp;


    std::vector<RenderManager> oneSkyBox;
    ComPtr<ID3D12Resource> mSkyboxCB;      // 리소스
    UINT8* mMappedSkyboxData = nullptr;    // 매핑된 포인터
    ComPtr<ID3D12RootSignature> m_skyboxRootSignature;
    ComPtr<ID3D12PipelineState> m_skyboxPSO;

    ComPtr<ID3D12Resource> m_skyVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_skyVertexBufferView;

    
    bool shouldGameClear = false;
    int gameoverOffset;
    int gameclearOffset;
};



