#include "Engine.h"
#define _CRT_SECURE_NO_WARNINGS
using namespace std;



Engine::Engine(UINT width, UINT height, std::wstring name, HWND hwnd) :
    DXSample(width, height, name),
    m_frameIndex(0),
    m_rtvDescriptorSize(0)
{

    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(width);
    m_scissorRect.bottom = static_cast<LONG>(height);

    m_hwnd = hwnd;
}


void Engine::OnInit()
{
    PathFinder& pt = PathFinder::GetInstance();
    pt.InitMap();
    GemUI& gem = GemUI::GetInstance();
    gem.initGem();

    RECT clientRect;
    if (GetClientRect(m_hwnd, &clientRect)) {
        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;
        std::cout << "Client area size: " << width << "x" << height << std::endl;
    }
    else {
        std::cerr << "Failed to get client rect." << std::endl;
    }

    for (int i = 0; i < 7; i++) {
        isLampOn.push_back(false);
    }
    keyHave.push_back(false);
    keyHave.push_back(false);
    keyHave.push_back(false);

    janitor = new Transform();
    janitor->defineCurrent();
    janitor->setName("janitor");
    janitor->finishedRotate(true);

    facmanNum = 0;
	LoadPipeline();
    LoadAssets();
    /*
    SceneManager& scene = SceneManager::GetInstance();
    scene.setswapchain(m_swapChain);
    setbitmaps(scene.getmaps());
    setclientRect(scene.getrect());
    */
    fpsInstance = initializeFPS();
    fpsInstance->calculateFrameStats();
    //std::vector<BoundingBox> dox;
    BoundingBox Extents;

    std::vector<std::reference_wrapper<BoundingBox>> dox;
    for (int i = 0; i < master2.size(); i++) {
        std::vector<Mainset>& main = master2[i].getTree();        
        for (auto& m : main) {  // main의 각 요소를 순회
            dox.push_back(m.Box);    // BoundingBox 추가
        }
    }

    janitor->setMapBox(dox);

    //사용안함
    std::vector<BoundingBox> dox2;
    for (int i = 0; i < keyAndDoor.size(); i++) {
        std::vector<Mainset> main = keyAndDoor[i].getTree();
        for (const auto& m : main) {  // main의 각 요소를 순회
            dox2.push_back(m.Box);    // BoundingBox 추가
        }
    }
    //여기까지
    cam = new Camera(fpsInstance, dox, dox2, width, height);
    janitor->setCampointer(cam);

    //바닥boundingbox설정
    RenderManager manager1;
    manager1.minmaxBase(L"asset\\houseshoese\\houseshoese.obj");
    cam->setBasement(manager1.getBasement());

    {      
       
        float screenWidth = width;
        float screenHeight = height;
        float uiWidth = screenWidth / 4.0f;
        float uiHeight = screenHeight / 4.0f;

        // UI 카메라 이동 (전체 화면에서 오른쪽 하단으로 이동)
        float offsetX = (screenWidth / 2.0f) - (uiWidth / 2.0f);
        float offsetY = -(screenHeight / 2.0f) + (uiHeight / 2.0f);

        cam->UICamera(50.0f, 50.0f, uiWidth, uiHeight, 0.0f, 1.0f);
    }
    
    //필요없는듯?
    //janitor->setBoundingBoxs(dox);

    isinitcomplete = true;
    OnRender();
}

void Engine::LoadPipeline()
{
#if defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {

        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

ComPtr<IDXGIFactory4> factory;
ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

if (m_useWarpDevice)
{
    ComPtr<IDXGIAdapter> warpAdapter;
    ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

    ThrowIfFailed(D3D12CreateDevice(
        warpAdapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)
    ));
}
else
{
    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(
        hardwareAdapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)
    ));
}

// Describe and create the command queue.
D3D12_COMMAND_QUEUE_DESC queueDesc = {};
queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

// Describe and create the swap chain.
DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
swapChainDesc.BufferCount = FrameCount;
swapChainDesc.BufferDesc.Width = m_width;
swapChainDesc.BufferDesc.Height = m_height;
swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER; // 백버퍼 설정//DXGI_USAGE_RENDER_TARGET_OUTPUT;
swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
swapChainDesc.OutputWindow = m_hwnd;//Win32Application::GetHwnd();
swapChainDesc.SampleDesc.Count = 1;
swapChainDesc.Windowed = TRUE;

ComPtr<IDXGISwapChain> swapChain;
ThrowIfFailed(factory->CreateSwapChain(
    m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
    &swapChainDesc,
    &swapChain
));

ThrowIfFailed(swapChain.As(&m_swapChain));

// This sample does not support fullscreen transitions.
//ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

// Create descriptor heaps.
{
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

// Create frame resources.
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV for each frame.
    for (UINT n = 0; n < FrameCount; n++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
        m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }
}

ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

void Engine::LoadAssets()
{
    CreateDepthBuffer();
    // Create an empty root signature.
    {

        // Root parameter can be a table, root descriptor or root constants.
        CD3DX12_ROOT_PARAMETER slotRootParameter[5];

        // World-View-Projection
        CD3DX12_DESCRIPTOR_RANGE cbvTable;
        cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // CBV 0번 슬롯
        slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

        // 재질정보
        CD3DX12_DESCRIPTOR_RANGE cbvTable1;
        cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); // CBV 1번 슬롯
        slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);


        // 맵에 있는 전등 on off 여부
        CD3DX12_DESCRIPTOR_RANGE cbvTable2;
        cbvTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2); // CBV 2번 슬롯
        slotRootParameter[2].InitAsDescriptorTable(1, &cbvTable2);


        // 텍스쳐 설정
        CD3DX12_DESCRIPTOR_RANGE srvTable;
        srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        slotRootParameter[3].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

        // 샘플러 설정
        CD3DX12_DESCRIPTOR_RANGE samplerTable;
        samplerTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        slotRootParameter[4].InitAsDescriptorTable(1, &samplerTable, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(5, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        //첫번째 인수 루트개수

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },  // 위치
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },    // 법선
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },        // 텍스처 좌표
        };

        
        CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
        // 렌더 타겟 0번에 알파 블렌딩 활성화
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
        // SrcColor = SrcAlpha * SrcColor, DestColor = (1-SrcAlpha) * DestColor
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        // 알파 채널은 그대로 쓰기
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


        // 1) DepthStencilState를 기본값으로 초기화
        CD3DX12_DEPTH_STENCIL_DESC dsDesc(D3D12_DEFAULT);

        // 2) 깊이 테스트는 유지, 깊이 쓰기만 끔
        dsDesc.DepthEnable = TRUE;                             // 깊이 테스트 켜기
        dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;      // 깊이 쓰기 끄기
        dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 또는 LESS도 OK

        // (스텐실은 안 쓰면 기본값 그대로 둡니다)
        dsDesc.StencilEnable = FALSE;



        // 2) PSO에 대입
        //psoDesc.BlendState = blendDesc;

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = blendDesc;
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        //psoDesc.DepthStencilState = dsDesc;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 깊이값이 작을수록 가까움
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이-스텐실 포맷 지정
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));



    }

    //UI용 루트서술자와 파이프라인 설정
    CreateUIRootSignature();
    CreateUIPipelineState();


    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());

    //카메라 상수버퍼 생성
    BuildCameraConstantBuffer();
    //샘플러 생성
    CreateSampler();
    //텍스쳐가 없는 물체를 그릴때 쓸 빈 SRV 디스크립터
    CreateEmptyDescriptor();

    CameraworldBuffer();

    //상수버퍼, 텍스쳐버퍼를 관리하는데 사용하는 오프셋
    //처음 4 개는 cbv (카메라)    cbv(UI카메라)  srv대신 쓸(빈 디스크립터)  cbv(카메라 world)가 차지
    int globalmaterialoffset = 4;

    //masterCount = 0;

    
    
    using index_t = uint16_t;        
    const wchar_t* filename = L"AssetList.txt";

    // BUFFER_SIZE 크기의 wchar_t 배열로 버퍼 선언
    const int BUFFER_SIZE = 1024;
    wchar_t m_filename[BUFFER_SIZE]; 

    // UTF-16LE로 파일 열기
    FILE* file = nullptr;
    if (_wfopen_s(&file, filename, L"rt, ccs=UTF-16LE") != 0 || !file) {
        MessageBox(NULL, TEXT("에셋 파일 경로 오류"), TEXT("알림"), MB_OK);
        return;
    }
    
    while (fgetws(m_filename, BUFFER_SIZE, file)) {
        
        // 개행 문자 제거
        size_t len = wcslen(m_filename);
        if (len > 0 && m_filename[len - 1] == L'\n') {
            m_filename[len - 1] = L'\0';
        }

        RenderManager manager0(
            m_device.Get(), cbvHandle, mCbvSrvDescriptorSize, globalmaterialoffset,
            m_viewport, m_pipelineState, m_commandQueue.Get(), m_swapChain.Get());

        manager0.Load(m_filename);
        globalmaterialoffset = manager0.getOffset();
        /*
        master.push_back(manager0);  // 기본 저장
        master2.push_back(manager0);
        globalmaterialoffset = manager0.getOffset();
        */
        if (wcsstr(m_filename, L"janitor")) {
            pacman.push_back(manager0);
            master2.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"houseshoese")) {
            master.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"manypoly")) {
            master.push_back(manager0);
            master2.push_back(manager0);
            rockTable.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gem0")) {
            master.push_back(manager0);
            PickTable.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"fishh")) {
            master.push_back(manager0);
            UIoffset = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"fish1")) {
            master.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"non")) {
            master.push_back(manager0);
            non = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gamestart")) {
            master.push_back(manager0);
            gameStart = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gameend")) {
            master.push_back(manager0);
            gameEnd = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"Skybox")) {
            master.push_back(manager0);
            oneSkyBox.push_back(manager0);
            skyBox = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gemcount0")) {
            master.push_back(manager0);
            gemCount.push_back(manager0);
            Countstart = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gemcount")) {
            master.push_back(manager0);
            gemCount.push_back(manager0);
            gemCount.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"compactgem")) {
            master.push_back(manager0);
            manager0.setfAlpha0();
            compactGem.push_back(manager0);
            compactGemMasterOffset = master.size() - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"lamp7")) {
            master.push_back(manager0);
            PickTableLamp.push_back(manager0);
            //globalmaterialoffset = manager0.getOffset();
            Lampstart = globalmaterialoffset;
            goto notGetglobaloffset;
        }
        else if (wcsstr(m_filename, L"lamp")) {
            master.push_back(manager0);
            PickTableLamp.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"lantern")) {
            master.push_back(manager0);
            Lamp.push_back(manager0);
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"gameover")) {
            master.push_back(manager0);            
            gameoverOffset = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else if (wcsstr(m_filename, L"escape")) {
            master.push_back(manager0);            
            gameclearOffset = globalmaterialoffset - 1;
            goto goNextasset;
        }
        else {
            master.push_back(manager0);
            master2.push_back(manager0);
        }

    goNextasset:
        //globalmaterialoffset = manager0.getOffset();

    notGetglobaloffset:
        ; // 빈 문장이지만 label을 위해 필요
    }

    fclose(file); 

    
       
//프레임당 하나의 몬스터 프레임 버전을 출력할 것으로
//일단 0번째 프레임버전의 몬스터를 출력벡터에 저장
    master.push_back(pacman[0]);    
        
    //직교투영을 쓰지 않고 처음 화면에 보이도록 하는 좌표의 사각형 생성
        CreateUIVertexBuffer();        
        
    //직교 투영버전의 사각형 생성
    //게임 시작화면 UI생성하는데 사용
        createOrthographSquare();
        createOrthographRootsignature();
        createOrthographConstantBuffer();
        CreateOrthographPipelineState();

   //Skybox구현   
        createSkyBox();
        createSkyBoxConstantBuffer();
        createSkyBoxRootsignature();
        createSkyBoxPipelineState();   
    

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }
}

void Engine::OnUpdate(float deltaX, float deltaY)
{
        GemUI& gem = GemUI::GetInstance();
        fpsInstance->calculateFrameStats();
        cam->CameraUpdate(deltaX, deltaY);
        
        //P버튼 누르면 손전등 활성화
        if (GetAsyncKeyState('P') & 0x8000) {     
            if (ishaveLamp) {
                ishaveLamp = false;
            }
            else {
                ishaveLamp = true;
            }
        }



        //Pick(deltaX, deltaY);
        //화면 정중앙 계산
        
        int width;
        int height;
        RECT clientRect;
        if (GetClientRect(m_hwnd, &clientRect)) {
            width = clientRect.right - clientRect.left;
            height = clientRect.bottom - clientRect.top;
            std::cout << "Client area size: " << width << "x" << height << std::endl;
        }
        else {
            std::cerr << "Failed to get client rect." << std::endl;
        }
        
        //처음 시작화면에서 마우스 클릭당시의 커서 위치를 계산하여
        //어느 버튼을 눌렀는지 판단
        if (isMenu) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_hwnd, &pt);            
            int x = pt.x * 477 / width;
            int y = pt.y * 388 / height;

            if (x > 145 && x < 330 && y>155 && y < 200) {
                cursorOnStart = true;
                if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    isMenu = false;
                    shouldMouseCenter = true;
                }
            }
            else {
                cursorOnStart = false;
            }
            if (x > 150 && x < 310 && y>225 && y < 270) {
                cursorOnEnd = true;                
                if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    PostMessage(m_hwnd, WM_CLOSE, 0, 0);
                }
            }
            else {
                cursorOnEnd = false;
            }
        }

        //게임 실행도중 ESC키를 누르면 처음 시작화면 송출
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {            
            if (pastEsc) {
                isMenu = false;
                pastEsc = false;
                shouldMouseCenter = true;
            }
            else {
                isMenu = true;
                pastEsc = true;
                shouldMouseCenter = false;
            }
        }

        POINT centerPoint;
        centerPoint.x = width / 2;
        centerPoint.y = height / 2;
        //ClientToScreen(m_hwnd, &centerPoint);
        //

        bool isMousePressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000); // 현재 마우스 상태
        // 눌렸다가 떼진 상태 (클릭 완료)
        if (wasMousePressed && !isMousePressed)
        {        
            //어느 물체를 클릭했는지 레이트레이싱
            Pick(centerPoint.x, centerPoint.y);
        }
        wasMousePressed = isMousePressed; // 현재 상태를 다음 프레임을 위해 저장

        gem.updateGem();//플레이어가 보석 범위내에 있는지 확인
        shouldUI = gem.getShouldUI();//보석위의 화살표 렌더할지 말지 결정

        
        if (gem.getGem1State()) {//보석1이 위치내에 있을때
            XMFLOAT3 mPosition = { -13.6f, 1.7f,-31.9f };//보석1의 위치
            if (isLookObject(mPosition) == false) {//플레이어가 보석을 바라보지 않고있을때는 화살표 UI를 출력하지말아야함
                shouldUI = false;
            }
            else {
                XMMATRIX mView = cam->GetView();
                XMMATRIX mProj = cam->GetProj();
                XMMATRIX viewProj = mView * mProj;
                whatGem = gem.WorldToScreen(mPosition, viewProj, width, height);//whatGem는 추후 렌더링 할 화면상 좌표     
            }
        }
        else if (gem.getGem2State()) {
            XMFLOAT3 mPosition = { -1.3f, 1.7f,-19.7f };//보석2의 위치
            if (isLookObject(mPosition) == false) {
                shouldUI = false;
            }
            else {
                XMMATRIX mView = cam->GetView();
                XMMATRIX mProj = cam->GetProj();
                XMMATRIX viewProj = mView * mProj;
                whatGem = gem.WorldToScreen(mPosition, viewProj, width, height);                
            }
        }
        else if(gem.getGem3State()){
            XMFLOAT3 mPosition = { 5.0f, 1.7f,-19.0f };//보석3의 위치
            if (isLookObject(mPosition) == false) {
                shouldUI = false;
            }
            else {
                XMMATRIX mView = cam->GetView();
                XMMATRIX mProj = cam->GetProj();
                XMMATRIX viewProj = mView * mProj;
                whatGem = gem.WorldToScreen(mPosition, viewProj, width, height);                
            }
        }
        else {//범위내에 아무 보석없음
            shouldUI = false;
        }

        
        /*521
        for (RenderManager manager : PickTable) {
            std::vector <Mainset> Tree = manager.getTree();

            for (int i = 0; i < Tree.size(); i++) {
                BoundingBox box = Tree[i].Box;
            }
        }
        */
        //if (GetAsyncKeyState('P') & 0x8000) {

        //상수버퍼 업로드
            {
                Camera::ObjectConstants objConstants = cam->getwoldViewProj();                
                constantUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
                CopyData(0, objConstants); // 데이터를 objConstants로 설정
                constantUploadBuffer->Unmap(0, nullptr);
            }

            //skyBox
            {
                XMMATRIX world = XMMatrixIdentity(); // 또는 위치 상관없는 작은 큐브

                // 카메라 View에서 위치 제거
                //플레이어가 항상 가운데에 위치 그리고 회전만 반영
                XMMATRIX view = cam->GetView();
                view.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

                // 투영 행렬
                XMMATRIX proj = cam->GetProj();
                XMMATRIX viewProj = XMMatrixMultiply(proj, view);
                XMMATRIX wvp = XMMatrixTranspose(viewProj);//HLSL는 열우선으로 동작

                //상수버퍼 업로드
                SkyboxConstants skyData;
                skyData.SkyViewProj = wvp;
                memcpy(mMappedSkyboxData, &skyData, sizeof(SkyboxConstants));
            }
            
            //Camworld 구조체는 카메라의 월드좌표, 플레이어의 손전등 유무, 맵에있는 전등 켜짐 유무를 포함한다
            {
                UINT matBufferSize = sizeof(CamWorld);
                CamWorld world;
                world.gCameraPos = cam->getmyWorld();
                if (ishaveLamp) {//손전등을 들고 있는경우
                    world.Lamp = 1.0f;
                }
                else {//아닌경우
                    world.Lamp = 0.0f;
                }
                
                //Pick함수에서 업데이트한 벡터값을 구조체에 반영
                //1이 켜진것이고 0은 꺼진상태를 나타냄
                for (int i = 0; i < isLampOn.size(); i++) {
                    if (i == 0) {
                        if (isLampOn[i] == true) {
                            world.l1 = 1;
                        }
                        else {
                            world.l1 = 0;
                        }
                    }else if (i == 1) {
                        if (isLampOn[i] == true) {
                            world.l2 = 1;
                        }
                        else {
                            world.l2 = 0;
                        }
                    }
                    else if (i == 2) {
                        if (isLampOn[i] == true) {
                            world.l3 = 1;
                        }
                        else {
                            world.l3 = 0;
                        }
                    }
                    else if (i == 3) {
                        if (isLampOn[i] == true) {
                            world.l4 = 1;
                        }
                        else {
                            world.l4 = 0;
                        }
                    }
                    else if (i == 4) {
                        if (isLampOn[i] == true) {
                            world.l5 = 1;
                        }
                        else {
                            world.l5 = 0;
                        }
                    }
                    else if (i == 5) {
                        if (isLampOn[i] == true) {
                            world.l6 = 1;
                        }
                        else {
                            world.l6 = 0;
                        }
                    }
                    else if (i == 6) {
                        if (isLampOn[i] == true) {
                            world.l7 = 1;
                        }
                        else {
                            world.l7 = 0;
                        }
                    }
                }              
                //패딩
                world.padding = 0;
                
                // 데이터 복사
                void* mappedData;
                CameraWorldBuffer->Map(0, nullptr, &mappedData);
                memcpy(mappedData, &world, matBufferSize);
                CameraWorldBuffer->Unmap(0, nullptr);
            }

        createOrthographMatrix();//직교투영행렬 생성후 GPU에 업로드
        OnRender();//몬스터의 SRT이동 관리    
}

void Engine::OnRender()
{
    frameCount++;  // 매프레임 마다 1 증가
    if (frameCount >= frameDelay) {
        frameCount = 0;
    }// 카운터 초기화

    if (frameCount == 0) {
        //몬스터는 움직임을 구현하기 위해 39개의 움직임을 가지고 있는데
        //facmanNum번째 인덱스의 몬스터 정점을 렌더링
        if (facmanNum < 39) {
            facmanNum++;
        }
        else {
            facmanNum = 0;
        }

        
        //frameCount가 0이 될때마다 39개의 정점정보를 SRT 시킨다.
        if(frameCount==0){
            janitor->finishedTrans(true);
            //janitor라는 몬스터를 이동시킨다.
            for (int i = 0; i < 39; i++) {
               pacman[i].DynamicVertex(janitor);
            }        
            //janitor->finishedTrans(true);
            //janitor->finishedRotate(true);
            
            //39개중에 0번째(임의의 수) 몬스터 정점정보를 가지고 충돌체 상자를 구현한다
            BoundingBox once = pacman[0].getBoundingBox();
            cam->setHimhitbox(once);

            //몬스터가 이동 후 지점을 기준으로 범위 수정
            //범위내에 있으면 몬스터가 추격
            BoundingBox range = janitor->getRange();
            cam->setRange(range);

            //janitor->setComingstate(false);521
        }
        //master.pop_back();
        //master.push_back(pacman[facmanNum]);//마찬가지로

    }

    //랜더링 할 몬스터 움직임 정점정보 변경
    master.pop_back();
    master.push_back(pacman[facmanNum]);//facmanNum 0~38
    
   

    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the framer
    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void Engine::PopulateCommandList()
{
    

    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocator->Reset());
    
    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.

    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    /*
    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
    */

    // Indicate that the back buffer will be used as a render target.
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &barrier);
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    // 깊이-스텐실 뷰 가져오기
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    

    ID3D12DescriptorHeap* heaps[] = { mCbvSrvHeap.Get(), mSamplerDescriptorHeap.Get()}; // 통합된 CBV_SRV_UAV + Sampler 힙 + 인덱스
    m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
    
    // Record commands.
    
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 0.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
   // m_commandList->IASetIndexBuffer(&indicesBufferView);
    //skyboxRender();


    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);


    m_commandList->SetGraphicsRootDescriptorTable(0, mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart()); // CBV 원래 3d 카메라
    m_commandList->SetGraphicsRootDescriptorTable(4, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());//sampler
    m_commandList->SetGraphicsRootDescriptorTable(2, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 3, mCbvSrvDescriptorSize));//카메라 월드좌표 상수버퍼
    
    //오프셋
    int footprint = 3;

    //master 객체는 렌더링할 RenderManager 객체들을 포함
    for (int k = 0; k < master.size(); k++)
    {
        RenderManager m = master[k];
        //손전등은 UI로 여기서 랜더링 하지 않으므로 손전등에 해당하는 CBV SRV 오프셋 건너뛰기 
        if (m.getIsLamp()) {
            footprint += 4;
            continue;
        }//Skybox.obj는 여기서 렌더링 하지 않으므로 CBV SRV 두개 건너뛰기
        else if (m.getFileName() == L"Skybox.obj") {
            footprint += 2;
            continue;
        }
                
        //Mainset 구조체 >> 인덱스 버퍼,뷰 충돌체
        vector<Mainset> main = m.getTree();
        //Subset 구조체 >> 재질버퍼, isTexture<-텍스쳐 유무
        vector<Subset> sub = m.getLeaf();

        m_commandList->IASetVertexBuffers(0, 1, &m.getVertexBufferView());
        
        for (int i = 0; i < main.size(); i++) {
            Mainset indices = main[i];
            Subset materialTex = sub[i];
            if (materialTex.istexture) {//텍스쳐를 가지고 있는경우                           
                m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint + 1, mCbvSrvDescriptorSize));
                footprint++;                
               m_commandList->SetGraphicsRootDescriptorTable(3, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint + 1, mCbvSrvDescriptorSize));
                
            }
            else {//텍스쳐를 가지지 않는 경우 텍스쳐 자리에 빈 디스크립터 할당
                m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint + 1, mCbvSrvDescriptorSize));
                m_commandList->SetGraphicsRootDescriptorTable(3, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 2, mCbvSrvDescriptorSize));//1을 2로 바꿈 3,28
            }
            m_commandList->IASetIndexBuffer(&indices.indicesBufferView);
            //m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint +1 , mCbvSrvDescriptorSize));
            m_commandList->DrawIndexedInstanced(indices.indicesCount, 1, 0, 0, 0);
            footprint++;
        }
    }
    
    
   

    //게임 시작화면 렌더링
    if (shouldUI) {
        RenderUI();
    }
    
    //손전등 렌더링
    if (ishaveLamp) {
        RenderLamp();
    }

    //우측 상단 보석 개수 UI 렌더링
    RenderRockCount();

        

    //Skybox 렌더링
    skyboxRender();


    //몬스터한테 잡히거나 게임클리어한경우
    if (cam->getGameOver() || shouldGameClear) {
        isMenu = true;
    }

    //게임 시작메뉴 렌더링    
    if (isMenu) {
        renderStartUI();
    }

    // Indicate that the back buffer will now be used to present.
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(m_commandList->Close());
}

void Engine::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. More advanced samples 
    // illustrate how to use fences for efficient resource usage.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Engine::OnDestroy()
{

    // Wait for the GPU to be done with all resources.
    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
}

void Engine::BuildCameraConstantBuffer()
{    
    UINT mElementByteSize = CalcConstantBufferByteSize(sizeof(Camera::ObjectConstants));
    UINT elementCount = 1;
    mCbvSrvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantUploadBuffer)));

    ThrowIfFailed(constantUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));

    UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(Camera::ObjectConstants));
   
    //UI카메라------------------------------
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeof(Camera::ObjectConstants);
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //521??524
    // UI 카메라용 상수 버퍼 생성
    m_device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&constantUploadBufferUI)
    );
    
    ThrowIfFailed(constantUploadBufferUI->Map(0, nullptr, reinterpret_cast<void**>(&mMappedDataUI)));
    
    //-------------------------------------------


    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
    cbvHeapDesc.NumDescriptors = 5000;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvSrvHeap)));
    cbvHandle = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
    //여까지

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = constantUploadBuffer->GetGPUVirtualAddress();//mObjectCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(Camera::ObjectConstants));

    m_device->CreateConstantBufferView(
        &cbvDesc,
        cbvHandle);//mCbvHeap->GetCPUDescriptorHandleForHeapStart());


    //5.21 ???524
    
    //UI용 카메라
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc2 = {};
        cbvDesc2.SizeInBytes = CalcConstantBufferByteSize(sizeof(Camera::ObjectConstants));//objCBByteSize;

        // UI 카메라용 CBV
        cbvDesc2.BufferLocation = constantUploadBufferUI->GetGPUVirtualAddress();
        D3D12_CPU_DESCRIPTOR_HANDLE cbvHandleUI;
        cbvHandleUI = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHandle, 1, mCbvSrvDescriptorSize);
        m_device->CreateConstantBufferView(&cbvDesc2, cbvHandleUI);
        //cbv2Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHandle, offset, mCbvSrvDescriptorSize);
        //------------------------------------------------
    }
        
}


UINT Engine::CalcConstantBufferByteSize(UINT byteSize)
{
    // 256바이트의 배수로 정렬
    return (byteSize + 255) & ~255;
}

//카메라용 상수버퍼 업로드
void Engine::CopyData(int elementIndex, Camera::ObjectConstants& data)
{
    memcpy(&mMappedData[elementIndex * CalcConstantBufferByteSize(sizeof(Camera::ObjectConstants))], &data, sizeof(Camera::ObjectConstants));
}


void Engine::CreateSampler()
{
    D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
    descHeapSampler.NumDescriptors = 1;
    descHeapSampler.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descHeapSampler.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = m_device->CreateDescriptorHeap(&descHeapSampler, IID_PPV_ARGS(&mSamplerDescriptorHeap));
    if (FAILED(hr)) {
        // 실패한 경우 HRESULT 값 확인
        std::wstring errorMessage = L"CreateDescriptorHeap failed with error code: " + std::to_wstring(hr);
        MessageBox(NULL, errorMessage.c_str(), L"Error", MB_OK);
    }    
       
    D3D12_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    m_device->CreateSampler(&samplerDesc,
        mSamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}



void Engine::ReleaseResources()
{
    // GPU 대기 (GPU가 작업 중이면 해제가 불가능할 수 있음)
    WaitForPreviousFrame();

    // SwapChain 해제 (더 이상 DX12에서 화면을 갱신하지 않도록)
    if (m_swapChain) {
        m_swapChain->SetFullscreenState(FALSE, nullptr); // 전체화면 모드 해제
        m_swapChain.Reset();
    }

    // 그래픽 리소스 해제
    for (int i = 0; i < FrameCount; ++i) {
        m_renderTargets[i].Reset();
    }

    m_commandAllocator.Reset();
    m_commandQueue.Reset();
    m_commandList.Reset();
    m_rootSignature.Reset();
    m_pipelineState.Reset();

    // Descriptor Heaps 해제
    m_rtvHeap.Reset();
    mCbvHeap.Reset();
    mCbvSrvHeap.Reset();
    srvHeap.Reset();

    // 업로드 버퍼 및 상수 버퍼 해제
    constantBuffer.Reset();
    mUploadBuffer.Reset();
    constantUploadBuffer.Reset();

    // 기타 리소스 해제
    m_vertexBuffer.Reset();
    indicesBuffer.Reset();

    // 동기화 객체 해제
    CloseHandle(m_fenceEvent);
    m_fence.Reset();

    // CPU-GPU 동기화
    m_device.Reset(); // 디바이스 해제

    // 모든 리소스 해제 완료
}


void Engine::CreateDepthBuffer()
{
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0; //반드시 0으로 설정 (자동 정렬)
    depthStencilDesc.Width = m_width;  // 화면 너비
    depthStencilDesc.Height = m_height; // 화면 높이
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이 24bit, 스텐실 8bit
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // Layout을 UNKNOWN으로 설정
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthOptimizedClearValue;
    depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_depthStencilBuffer)
    ));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));


    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());  

}

void Engine::RenderUI()
{

    //test
    D3D12_VIEWPORT mainViewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    D3D12_RECT mainScissorRect = { 0, 0, (LONG)width, (LONG)height };
    float uiWidth = 100;
    float uiHeight = 100;

    // 화면상에서 UI를 표시할 위치 (UI 중심이 screenPos가 되도록 함)
    float uiX = whatGem.x - uiWidth * 0.5f;
    float uiY = whatGem.y - uiHeight * 0.5f;

    // UI 뷰포트 설정: (uiX, uiY)에서 uiWidth, uiHeight 크기로 정의
    D3D12_VIEWPORT uiViewport = {
        uiX,
        uiY,
        uiWidth,
        uiHeight,
        0.0f, 1.0f
    };

    // 스키저 Rect도 동일한 영역으로 설정 (정수 좌표로 변환)
    D3D12_RECT uiScissorRect = {
        static_cast<LONG>(uiX),
        static_cast<LONG>(uiY),
        static_cast<LONG>(uiX + uiWidth),
        static_cast<LONG>(uiY + uiHeight)
    };

    /*
    D3D12_VIEWPORT uiViewport = {
        width - uiWidth, // 우측 하단 위치
        height - uiHeight,
        uiWidth, uiHeight,
        0.0f, 1.0f
    };

    D3D12_RECT uiScissorRect = {
        (LONG)(width - uiWidth), (LONG)(height - uiHeight),
        (LONG)width, (LONG)height
    };
    */
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


    // UI 전용 파이프라인 상태 설정
    m_commandList->SetPipelineState(m_uiPipelineState.Get());
    m_commandList->SetGraphicsRootSignature(m_uiRootSignature.Get());

    // UI용 뷰포트 & Scissor Rect 설정 (이미 위에서 설정됨)
    m_commandList->RSSetViewports(1, &uiViewport);
    m_commandList->RSSetScissorRects(1, &uiScissorRect);

    // UI용 Descriptor Table 설정
    
     m_commandList->SetGraphicsRootDescriptorTable(0,
         CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), UIoffset, mCbvSrvDescriptorSize)); // UI용 텍스쳐
    
    m_commandList->SetGraphicsRootDescriptorTable(1, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());//sampler
    
    // UI Quad (2D 사각형) 그리기
    m_commandList->IASetVertexBuffers(0, 1, &m_uiVertexBufferView);
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->DrawInstanced(6, 1, 0, 0); // UI는 보통 2개의 삼각형으로 화면을 덮음
}

void Engine::CreateUIVertexBuffer()
{
    UIVertex uiQuadVertices[] = {
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } }, // 좌상단
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // 우상단
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // 좌하단
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // 좌하단
    { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // 우상단
    { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } }, // 우하단
    };

    const UINT vertexBufferSize = sizeof(uiQuadVertices);

    // Vertex Buffer 생성
    m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uiVertexBuffer));

    // Vertex Buffer 데이터 복사
    void* vertexData = nullptr;
    CD3DX12_RANGE readRange(0, 0); // CPU 읽기 불필요
    m_uiVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexData));
    memcpy(vertexData, uiQuadVertices, vertexBufferSize);
    m_uiVertexBuffer->Unmap(0, nullptr);

    // Vertex Buffer View 설정
    m_uiVertexBufferView.BufferLocation = m_uiVertexBuffer->GetGPUVirtualAddress();
    m_uiVertexBufferView.StrideInBytes = sizeof(UIVertex);
    m_uiVertexBufferView.SizeInBytes = vertexBufferSize;
}

void Engine::CreateUIRootSignature()
{

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER rootParameters[2];

    // 텍스쳐 설정
    CD3DX12_DESCRIPTOR_RANGE srvTable;
    srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    rootParameters[0].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

    // 샘플러 설정
    CD3DX12_DESCRIPTOR_RANGE samplerTable;
    samplerTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
    rootParameters[1].InitAsDescriptorTable(1, &samplerTable, D3D12_SHADER_VISIBILITY_PIXEL);


    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(2, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    //첫번째 인수 루트개수

   
    // Root Signature 직렬화 및 생성
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        throw std::runtime_error("Failed to serialize UI Root Signature.");
    }

    hr = m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_uiRootSignature));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create UI Root Signature.");
    }
    
}

void Engine::CreateUIPipelineState()
{
    ComPtr<ID3DBlob> m_uiVertexShader;
    ComPtr<ID3DBlob> m_uiPixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

   
    ThrowIfFailed(D3DCompileFromFile(L"UI.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &m_uiVertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"UI.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &m_uiPixelShader, nullptr));

    // Input Layout 정의 (UI는 2D이므로 간단한 구조)
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // 그래픽 파이프라인 상태 객체(PSO) 설정
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = m_uiRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_uiVertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_uiPixelShader.Get());

    // 블렌딩 설정 (알파 블렌딩 활성화)
    D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
    blendDesc.BlendEnable = TRUE;
    blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.BlendState.RenderTarget[0] = blendDesc;

    // 래스터라이저 설정 (UI는 보통 컬링 없음)
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    //psoDesc.RasterizerState.CullMode = D3D12_CULL_NONE; // UI는 컬링 비활성화

    // 깊이/스텐실 상태 설정 (UI는 깊이 버퍼 필요 없음)
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE; // 깊이 테스트 비활성화
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN; // 깊이 버퍼 사용 안 함

    // 렌더 타겟 설정 
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // PSO 생성
    HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_uiPipelineState));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create UI Pipeline State.");
    }
}

void Engine::CreateEmptyDescriptor()
{
    // 1. NULL 서술자 정의
    D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
    nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    nullSrvDesc.Texture2D.MipLevels = 1;

    // 2. CPU 디스크립터 힙에서 빈 서술자를 만들 위치 선택
    D3D12_CPU_DESCRIPTOR_HANDLE cpuEmptyHandle =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), 2, mCbvSrvDescriptorSize);

    // 3. NULL 서술자를 CPU 힙에 생성
    m_device->CreateShaderResourceView(nullptr, &nullSrvDesc, cpuEmptyHandle);


    // 6. 셰이더에서 사용할 GPU 디스크립터 핸들 저장
    m_nullSrvGpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
        mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 2, mCbvSrvDescriptorSize
    );
}

void Engine::Pick(float x, float y)
{
    XMFLOAT4X4 P = cam->GetProj4x4f();//cam->getwoldViewProj().WorldViewProj;

    float vx = (+2.0f * x / width - 1.0f) / P(0, 0);
    float vy = (-2.0f * y / height + 1.0f) / P(1, 1);

    //MVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); 
    XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); //cam->getMyPosition();//XMLoadFloat3(cam->getMyPosition());
    XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

    XMMATRIX V = cam->GetView();
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

    // 세계 좌표계에서의 레이 방향 구하기
    rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
    rayDir = XMVector3TransformNormal(rayDir, invView);    
    rayDir = XMVector3Normalize(rayDir);  

    //맵에 있는 보석에 대해 레이트레이싱
    for (int j = 0; j < PickTable.size(); j++){
        if (keyHave[j] == true) {//이미 PickTable배열에 해당하는 보석을 얻은경우
            continue;
        }
        std::vector <Mainset> Tree = PickTable[j].getTree();
        for (int i = 0; i < Tree.size(); i++) {
            BoundingBox box = Tree[i].Box;
            
            float tmin = 10.0f;
            bool intersects = box.Intersects(rayOrigin, rayDir, tmin);
            std::cout << "Intersects: " << intersects << ", tmin: " << tmin << std::endl;

            XMVECTOR vCenter = XMLoadFloat3(&box.Center);
            XMVECTOR vExtents = XMLoadFloat3(&box.Extents);
            bool isInside = XMVector3InBounds(rayOrigin, vExtents);
            std::cout << "Is Ray Origin Inside Box?: " << isInside << std::endl;


            if (box.Intersects(rayOrigin, rayDir, tmin))
            {
                // 충돌한 경우 추가 처리
                GemUI& gem = GemUI::GetInstance();                
                if (j == 0) {
                    gem.setGem1State(false);
                    PickTable[j].setfAlpha0();
                }
                else if (j == 1) {
                    gem.setGem2State(false);
                    PickTable[j].setfAlpha0();
                }
                else {
                    gem.setGem3State(false);
                    PickTable[j].setfAlpha0();
                }
                keyHave[j] = true;
                gem.setgemCount();
            }

        }

    }
    //맵 에 있는 전등에대해 레이트레이싱
    for (int k = 0; k < PickTableLamp.size(); k++) {
        RenderManager manager = PickTableLamp[k];
        std::vector <Mainset> Tree = manager.getTree();

        for (int i = 0; i < Tree.size(); i++) {
            BoundingBox box = Tree[i].Box;

            float tmin = 10.0f;
            bool intersects = box.Intersects(rayOrigin, rayDir, tmin);
            std::cout << "Intersects: " << intersects << ", tmin: " << tmin << std::endl;

            XMVECTOR vCenter = XMLoadFloat3(&box.Center);
            XMVECTOR vExtents = XMLoadFloat3(&box.Extents);
            bool isInside = XMVector3InBounds(rayOrigin, vExtents);
            std::cout << "Is Ray Origin Inside Box?: " << isInside << std::endl;


            if (box.Intersects(rayOrigin, rayDir, tmin))
            {
                // 충돌한 경우 추가 처리                
                if (isLampOn[k] == false) {
                    isLampOn[k] = true;
                }
                else {
                    isLampOn[k] = false;
                }
            }
        }
    }

    GemUI& gem = GemUI::GetInstance();
    int a = gem.getgemCount();

    //보석 3개를 가지고 시작지점 바위테이블을 클릭할 경우
    //테이블 위의 보석 알파값을 1로 세팅
    for (int j = 0; j < rockTable.size(); j++) {     

        std::vector <Mainset> Tree = rockTable[j].getTree();

        for (int i = 0; i < Tree.size(); i++) {
            BoundingBox box = Tree[i].Box;

            float tmin = 10.0f;
            bool intersects = box.Intersects(rayOrigin, rayDir, tmin);
            std::cout << "Intersects: " << intersects << ", tmin: " << tmin << std::endl;

            XMVECTOR vCenter = XMLoadFloat3(&box.Center);
            XMVECTOR vExtents = XMLoadFloat3(&box.Extents);
            bool isInside = XMVector3InBounds(rayOrigin, vExtents);
            std::cout << "Is Ray Origin Inside Box?: " << isInside << std::endl;


            if (box.Intersects(rayOrigin, rayDir, tmin))
            {
                // 충돌한 경우 추가 처리
                GemUI& gem = GemUI::GetInstance();           

                if (gem.getgemCount() == 3) {
                    compactGem[0].setfAlpha1();//알파값 1로 변경
                    shouldGameClear = true;//게임클리어 UI bool
                }
            }
        }
    }
}

//카메라 월드좌표 상수버퍼 생성
void Engine::CameraworldBuffer()
{
    int offset = 3;// 오프셋 3로 고정
    UINT matBufferSize = sizeof(CamWorld);
    UINT alignedSize = CalcConstantBufferByteSize(sizeof(CamWorld));

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize);

    m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&CameraWorldBuffer));
    //첫 복사 
    CamWorld world;
    world.gCameraPos = { 0.0f, 0.0f,10.0f };//cam->getmyWorld();


    // 데이터 복사
    void* mappedData;
    CameraWorldBuffer->Map(0, nullptr, &mappedData);

    memcpy(mappedData, &world, matBufferSize);
    CameraWorldBuffer->Unmap(0, nullptr);
    
    // GPU 가상 주소 저장
    CameraWorldBufferAddress = CameraWorldBuffer->GetGPUVirtualAddress();
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbv2Handle;

    cbv2Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHandle, offset, mCbvSrvDescriptorSize);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = CameraWorldBufferAddress;
    cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(CamWorld));

    m_device->CreateConstantBufferView(
        &cbvDesc,
        cbv2Handle);
}

//손전등 출력
void Engine::RenderLamp()
{    
    D3D12_VIEWPORT mainViewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    D3D12_RECT mainScissorRect = { 0, 0, (LONG)width, (LONG)height };
    float uiWidth = 400;
    float uiHeight = 400;

    
    float offsetX = 100;
    D3D12_VIEWPORT uiViewport = {
        //width - uiWidth, // 우측 하단 위치
        width - uiWidth - offsetX, // 왼쪽으로 이동
        height - uiHeight,
        uiWidth, uiHeight,
        0.0f, 1.0f
    };

    D3D12_RECT uiScissorRect = {
        (LONG)(width - uiWidth - offsetX), (LONG)(height - uiHeight),
        (LONG)width, (LONG)height
    };
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


    // UI 전용 파이프라인 상태 설정
    m_commandList->SetPipelineState(m_uiPipelineState.Get());
    m_commandList->SetGraphicsRootSignature(m_uiRootSignature.Get());

    // UI용 뷰포트 & Scissor Rect 설정 (이미 위에서 설정됨)
    m_commandList->RSSetViewports(1, &uiViewport);
    m_commandList->RSSetScissorRects(1, &uiScissorRect);

    // UI용 Descriptor Table 설정   
    m_commandList->SetGraphicsRootDescriptorTable(1, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());//sampler

    // UI Quad (2D 사각형) 그리기
    m_commandList->IASetVertexBuffers(0, 1, &Lamp[0].getVertexBufferView());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    int footprint = Lampstart+1;

    RenderManager m = Lamp[0];
    vector<Mainset> main = m.getTree();
    vector<Subset> sub = m.getLeaf();
    for (int i = 0; i < main.size(); i++) {
        Mainset indices = main[i];
        Subset materialTex = sub[i];
        if (materialTex.istexture) {       
            m_commandList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint, mCbvSrvDescriptorSize));
        }
        else {//없으면 non 디스립터 할당
            m_commandList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 2, mCbvSrvDescriptorSize));
        }
        m_commandList->IASetIndexBuffer(&indices.indicesBufferView);
        m_commandList->DrawIndexedInstanced(indices.indicesCount, 1, 0, 0, 0);
        footprint++;
        footprint++;
    }

}

//우상단 보석 카운트UI 출력
void Engine::RenderRockCount()
{

    //test
    D3D12_VIEWPORT mainViewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    D3D12_RECT mainScissorRect = { 0, 0, (LONG)width, (LONG)height };
    float uiWidth = 200;
    float uiHeight = 200;


    float offsetX = 100;
    D3D12_VIEWPORT uiViewport = {
        //width - uiWidth, // 우측 하단 위치
        width - uiWidth - offsetX, // 왼쪽으로 이동
        0.0f,
        uiWidth, uiHeight,
        0.0f, 1.0f
    };

    D3D12_RECT uiScissorRect = {
    (LONG)(width - uiWidth - offsetX), // left
    0,                                 // top (상단)
    (LONG)(width - offsetX),           // right
    (LONG)(uiHeight)                   // bottom
    };

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


    // UI 전용 파이프라인 상태 설정
    m_commandList->SetPipelineState(m_uiPipelineState.Get());
    m_commandList->SetGraphicsRootSignature(m_uiRootSignature.Get());

    // UI용 뷰포트 & Scissor Rect 설정 (이미 위에서 설정됨)
    m_commandList->RSSetViewports(1, &uiViewport);
    m_commandList->RSSetScissorRects(1, &uiScissorRect);

    // UI용 Descriptor Table 설정   
    m_commandList->SetGraphicsRootDescriptorTable(1, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());//sampler

    // UI Quad (2D 사각형) 그리기
    m_commandList->IASetVertexBuffers(0, 1, &m_uiVertexBufferView);//&gemCount[0].getVertexBufferView());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    int footprint = Countstart;
    GemUI& gem = GemUI::GetInstance();
    int gemScore = gem.getgemCount();
    RenderManager m = gemCount[gemScore];
    vector<Mainset> main = m.getTree();
    vector<Subset> sub = m.getLeaf();
    for (int i = 0; i < main.size(); i++) {
        Mainset indices = main[i];
        Subset materialTex = sub[i];
        if (materialTex.istexture) {            
            m_commandList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint+(gemScore*2), mCbvSrvDescriptorSize));
        }
        else {
            m_commandList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), footprint + (gemScore * 2), mCbvSrvDescriptorSize));//1을 2로 바꿈 3,28
        }        
      
        m_commandList->DrawInstanced(6, 1, 0, 0);
        

    }
}

//플레이어가 물체를 보고있을때 아닐때를 판단하는 함수
bool Engine::isLookObject(const XMFLOAT3& objectPos)
{
    // 카메라 위치와 앞 방향 얻기
    XMFLOAT3 camPos = cam->getMyPos(); // 카메라 위치    
    XMMATRIX view = cam->GetView(); //뷰행렬
    XMMATRIX invView = XMMatrixInverse(nullptr, view); //월드좌표계
    
    XMFLOAT3 forward;
    XMStoreFloat3(&forward, invView.r[2]); // r[2]가 Z축관련 방향

    // 오브젝트까지의 방향 벡터 계산
    XMVECTOR camPosVec = XMLoadFloat3(&camPos);
    XMVECTOR objectPosVec = XMLoadFloat3(&objectPos);
    XMVECTOR toObjectVec = XMVector3Normalize(objectPosVec - camPosVec);

    // 카메라의 forward 벡터
    XMVECTOR camForwardVec = XMVector3Normalize(XMLoadFloat3(&forward));

    // 내적 계산
    float dot = XMVectorGetX(XMVector3Dot(toObjectVec, camForwardVec));

    return dot > 0.0f;
}

void Engine::createOrthographSquare()
{

    RECT clientRect;
    if (GetClientRect(m_hwnd, &clientRect)) {
        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;
    }
    else {
        std::cerr << "Failed to get client rect." << std::endl;
    }
    float screenWidth = width;
    float screenHeight = height;
    
    // 예: 화면 전체를 덮는 UI 사각형
    UIVertex vertices[] =
    {
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },                     // 좌상단
        { { (float)screenWidth, 0.0f, 0.0f }, { 1.0f, 0.0f } },       // 우상단
        { { 0.0f, (float)screenHeight, 0.0f }, { 0.0f, 1.0f } },      // 좌하단

        { { (float)screenWidth, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { (float)screenWidth, (float)screenHeight, 0.0f }, { 1.0f, 1.0f } },
        { { 0.0f, (float)screenHeight, 0.0f }, { 0.0f, 1.0f } },
    };

    const UINT vertexBufferSize = static_cast<UINT>(sizeof(vertices));

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_OrthographVertexBuffer)));

    // Copy the triangle data to the vertex buffer.
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_OrthographVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, vertices, vertexBufferSize);
    m_OrthographVertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    m_OrthographVertexBufferView.BufferLocation = m_OrthographVertexBuffer->GetGPUVirtualAddress();
    m_OrthographVertexBufferView.StrideInBytes = sizeof(UIVertex);
    m_OrthographVertexBufferView.SizeInBytes = vertexBufferSize;
}

void Engine::createOrthographRootsignature()
{
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

    CD3DX12_DESCRIPTOR_RANGE samplerTable;
    samplerTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0); // s0

    CD3DX12_ROOT_PARAMETER rootParameters[3];

    // b0 (상수 버퍼)
    rootParameters[0].InitAsConstantBufferView(0);

    // t0 (텍스처)
    rootParameters[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

    // s0 (샘플러)
    rootParameters[2].InitAsDescriptorTable(1, &samplerTable, D3D12_SHADER_VISIBILITY_PIXEL);

    // 루트 시그니처 생성
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 직렬화 및 생성
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob));
    ThrowIfFailed(m_device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&m_OrthographRootSignature)));

}

//시작화면 UI
void Engine::renderStartUI()
{

    //test
    D3D12_VIEWPORT mainViewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    D3D12_RECT mainScissorRect = { 0, 0, (LONG)width, (LONG)height };
    float uiWidth = 200;
    float uiHeight = 200;


    // 화면 전체에 UI를 출력하고 싶다면 아래처럼 설정!
    D3D12_VIEWPORT uiViewport = {
        0.0f, 0.0f,             // Top-Left (x, y)
        static_cast<float>(width),  // Width
        static_cast<float>(height), // Height
        0.0f, 1.0f              // MinDepth, MaxDepth
    };

    D3D12_RECT uiScissorRect = {
        0, 0,                   // Left, Top
        static_cast<LONG>(width),  // Right
        static_cast<LONG>(height) // Bottom
    };

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);


    // UI 전용 파이프라인 상태 설정
    m_commandList->SetPipelineState(m_OrthographPipelineState.Get());
    m_commandList->SetGraphicsRootSignature(m_OrthographRootSignature.Get());

    // UI용 뷰포트 & Scissor Rect 설정 (이미 위에서 설정됨)
    m_commandList->RSSetViewports(1, &uiViewport);
    m_commandList->RSSetScissorRects(1, &uiScissorRect);

    // UI용 Descriptor Table 설정   
    m_commandList->SetGraphicsRootDescriptorTable(1, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());//sampler

    // UI Quad (2D 사각형) 그리기
    m_commandList->IASetVertexBuffers(0, 1, &m_OrthographVertexBufferView);//&gemCount[0].getVertexBufferView());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Root Parameter 0: 상수 버퍼 gWVP
    m_commandList->SetGraphicsRootConstantBufferView(0, m_cbWVP->GetGPUVirtualAddress());

    // Root Parameter 1: 텍스처
    
    if (shouldGameClear) {
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), gameclearOffset, mCbvSrvDescriptorSize));
    }
    else if (cam->getGameOver()) {
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), gameoverOffset, mCbvSrvDescriptorSize));
    }
    else if (cursorOnStart) {
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), gameStart, mCbvSrvDescriptorSize)); // t0
    }
    else if (cursorOnEnd) {
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), gameEnd, mCbvSrvDescriptorSize)); // t0
    }
    else {
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), non, mCbvSrvDescriptorSize)); // t0
    }
    // Root Parameter 2: 샘플러
    m_commandList->SetGraphicsRootDescriptorTable(2, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); // s0

    m_commandList->DrawInstanced(6, 1, 0, 0);


    
}

void Engine::createOrthographConstantBuffer()
{
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(CBData) + 255) & ~255);

    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_cbWVP)
    ));
}

void Engine::createOrthographMatrix()
{
    RECT clientRect;
    if (GetClientRect(m_hwnd, &clientRect)) {
        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;
    }
    else {
        std::cerr << "Failed to get client rect." << std::endl;
    }
    float screenWidth = width;
    float screenHeight = height;

    //직교투영행렬 생성
    XMMATRIX world = XMMatrixIdentity();
    XMMATRIX view = XMMatrixIdentity();
    XMMATRIX proj = XMMatrixOrthographicOffCenterLH(
        0.0f, screenWidth,
        screenHeight, 0.0f, // Y축 반전
        0.0f, 1.0f
    );

    CBData cbData;
    cbData.gWVP = XMMatrixTranspose(world * view * proj); // HLSL은 열우선

    // GPU 메모리에 업로드
    UINT8* pData;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_cbWVP->Map(0, &readRange, reinterpret_cast<void**>(&pData)));
    memcpy(pData, &cbData, sizeof(cbData));
    m_cbWVP->Unmap(0, nullptr);
}

void Engine::CreateOrthographPipelineState()
{
    D3D12_INPUT_ELEMENT_DESC uiInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(L"OrthographUI.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"OrthographUI.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { uiInputLayout, _countof(uiInputLayout) };
    psoDesc.pRootSignature = m_OrthographRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // 투명 UI를 위해 블렌딩 활성화
    psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_OrthographPipelineState)));
}

bool Engine::getshouldMouseCenter()
{
    return shouldMouseCenter;
}

void Engine::createSkyBoxConstantBuffer()
{
    // 버퍼 생성 시:
    UINT skyboxCBByteSize = CalcConstantBufferByteSize(sizeof(SkyboxConstants));
    m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(skyboxCBByteSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mSkyboxCB)
    );
    // 맵핑
    CD3DX12_RANGE readRange(0, 0);
    mSkyboxCB->Map(0, &readRange, reinterpret_cast<void**>(&mMappedSkyboxData));

}

void Engine::createSkyBoxPipelineState()
{
    
    D3D12_INPUT_ELEMENT_DESC inputLayout[] =
    {
        // POSITION (float3)
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,  
            0,                            
            0,                           
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        // UV (float2)
        {
            "TEXCOORD", 0,
            DXGI_FORMAT_R32G32_FLOAT,     
            0,                       
            D3D12_APPEND_ALIGNED_ELEMENT, 
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

        // 2. 쉐이더 로드
        
        ComPtr<ID3DBlob> errorBlob;      
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile(L"SkyBox.hlsl", nullptr, nullptr, "VS_Skybox", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile(L"SkyBox.hlsl", nullptr, nullptr, "PS_Skybox", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        D3D12_DEPTH_STENCIL_DESC dsDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;      // 쓰기 OFF
        dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;     // 무조건 통과

        // 3. 그래픽스 파이프라인 상태 객체 설정
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_skyboxRootSignature.Get(); // Skybox 전용 RootSignature
        psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //psoDesc.RasterizerState.FrontCounterClockwise = FALSE; // 기본 CW
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//D3D12_CULL_MODE_FRONT; // 내부가 보이도록 FRONT CULL
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = dsDesc;//CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        //psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // Skybox는 z = 1도 통과하게
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.SampleDesc.Count = 1;

        // 4. PSO 생성
        HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_skyboxPSO));
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create Skybox PSO");
        }
    }



void Engine::createSkyBoxRootsignature()
{
    // 새로운 루트 서술자 정의
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];

    // 1. Skybox ViewProj 상수버퍼 (b0) - ConstantBufferView 사용
    slotRootParameter[0].InitAsConstantBufferView(0); // b0 - Skybox ViewProj

    // 2. 큐브맵 텍스처 (t0)
    CD3DX12_DESCRIPTOR_RANGE srvTable;
    srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
    slotRootParameter[1].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

    // 3. 샘플러 (s0)
    CD3DX12_DESCRIPTOR_RANGE samplerTable;
    samplerTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0); // s0
    slotRootParameter[2].InitAsDescriptorTable(1, &samplerTable, D3D12_SHADER_VISIBILITY_PIXEL);

    // 루트 서술자 생성
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(3, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 루트 서술자 직렬화
    ID3DBlob* pOutBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        }
        //return hr;
    }

    // 루트 서술자 생성
    hr = m_device->CreateRootSignature(0, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(&m_skyboxRootSignature));
    pOutBlob->Release();
    if (FAILED(hr))
    {
        //return hr;
    }
}

void Engine::skyboxRender()
{
    
        // 1. 뷰포트와 시저 설정 (전체 화면 기준)
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
        D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

        m_commandList->RSSetViewports(1, &viewport);
        m_commandList->RSSetScissorRects(1, &scissorRect);

        // 2. 렌더 타겟 설정
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            m_frameIndex,
            m_rtvDescriptorSize);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        // 3. 파이프라인과 루트 시그니처 설정
        m_commandList->SetGraphicsRootSignature(m_skyboxRootSignature.Get());
        m_commandList->SetPipelineState(m_skyboxPSO.Get());
       

        // 4. 루트 파라미터 설정
        // b0: ViewProj 상수 버퍼
        m_commandList->SetGraphicsRootConstantBufferView(0, mSkyboxCB->GetGPUVirtualAddress());

        // t0: 큐브맵 SRV
        m_commandList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(
            mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), skyBox, mCbvSrvDescriptorSize));//skyBox 아마 텍스쳐 인덱스 맞겟지?

        // s0: 샘플러
        m_commandList->SetGraphicsRootDescriptorTable(2, mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        RenderManager skybox = oneSkyBox[0];
        vector<Mainset> main = skybox.getTree();
       Mainset indices = main[0];
        
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView= skybox.getVertexBufferView();
        D3D12_INDEX_BUFFER_VIEW indicesBufferView;

       

        // 5. 정점 버퍼 및 인덱스 버퍼 설정
        m_commandList->IASetVertexBuffers(0, 1, &m_skyVertexBufferView);
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 6. 드로우 호출
        m_commandList->DrawInstanced(36, 1, 0, 0);        
    }

void Engine::createSkyBox()
{
    static SkyVertex skyboxVertices[] =
    {
        // +X 면: atlas (0/3,0/2) ~ (1/3,1/2)
        {{ +1, +1, -1 }, { 0.0f / 3, 0.0f / 2 }},  // 왼위
        {{ +1, +1, +1 }, { 1.0f / 3, 0.0f / 2 }},
        {{ +1, -1, -1 }, { 0.0f / 3, 1.0f / 2 }},
        {{ +1, -1, -1 }, { 0.0f / 3, 1.0f / 2 }},
        {{ +1, +1, +1 }, { 1.0f / 3, 0.0f / 2 }},
        {{ +1, -1, +1 }, { 1.0f / 3, 1.0f / 2 }},

        // -X 면: atlas (1/3,0/2)~(2/3,1/2)
        {{ -1, +1, +1 }, { 1.0f / 3, 0.0f / 2 }},
        {{ -1, +1, -1 }, { 2.0f / 3, 0.0f / 2 }},
        {{ -1, -1, +1 }, { 1.0f / 3, 1.0f / 2 }},
        {{ -1, -1, +1 }, { 1.0f / 3, 1.0f / 2 }},
        {{ -1, +1, -1 }, { 2.0f / 3, 0.0f / 2 }},
        {{ -1, -1, -1 }, { 2.0f / 3, 1.0f / 2 }},

        // +Y 면: atlas (2/3,0/2)~(3/3,1/2)
        {{ -1, +1, +1 }, { 2.0f / 3, 0.0f / 2 }},
        {{ +1, +1, +1 }, { 1.0f,   0.0f / 2 }},
        {{ -1, +1, -1 }, { 2.0f / 3, 1.0f / 2 }},
        {{ -1, +1, -1 }, { 2.0f / 3, 1.0f / 2 }},
        {{ +1, +1, +1 }, { 1.0f,   0.0f / 2 }},
        {{ +1, +1, -1 }, { 1.0f,   1.0f / 2 }},

        // -Y 면: atlas (0/3,1/2)~(1/3,2/2)
        {{ -1, -1, -1 }, { 0.0f / 3, 1.0f / 2 }},
        {{ +1, -1, -1 }, { 1.0f / 3, 1.0f / 2 }},
        {{ -1, -1, +1 }, { 0.0f / 3, 2.0f / 2 }},
        {{ -1, -1, +1 }, { 0.0f / 3, 2.0f / 2 }},
        {{ +1, -1, -1 }, { 1.0f / 3, 1.0f / 2 }},
        {{ +1, -1, +1 }, { 1.0f / 3, 2.0f / 2 }},

        // +Z 면: atlas (1/3,1/2)~(2/3,2/2)
        {{ -1, +1, +1 }, { 1.0f / 3, 1.0f / 2 }},
        {{ +1, +1, +1 }, { 2.0f / 3, 1.0f / 2 }},
        {{ -1, -1, +1 }, { 1.0f / 3, 2.0f / 2 }},
        {{ -1, -1, +1 }, { 1.0f / 3, 2.0f / 2 }},
        {{ +1, +1, +1 }, { 2.0f / 3, 1.0f / 2 }},
        {{ +1, -1, +1 }, { 2.0f / 3, 2.0f / 2 }},

        // -Z 면: atlas (2/3,1/2)~(3/3,2/2)
        {{ +1, +1, -1 }, { 2.0f / 3, 1.0f / 2 }},
        {{ -1, +1, -1 }, { 3.0f / 3, 1.0f / 2 }},
        {{ +1, -1, -1 }, { 2.0f / 3, 2.0f / 2 }},
        {{ +1, -1, -1 }, { 2.0f / 3, 2.0f / 2 }},
        {{ -1, +1, -1 }, { 3.0f / 3, 1.0f / 2 }},
        {{ -1, -1, -1 }, { 3.0f / 3, 2.0f / 2 }},
    };

        const UINT vertexBufferSize = sizeof(skyboxVertices);

        // Vertex Buffer 생성
        m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_skyVertexBuffer));

        // Vertex Buffer 데이터 복사
        void* vertexData = nullptr;
        CD3DX12_RANGE readRange(0, 0); // CPU 읽기 불필요
        m_skyVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexData));
        memcpy(vertexData, skyboxVertices, vertexBufferSize);
        m_skyVertexBuffer->Unmap(0, nullptr);

        // Vertex Buffer View 설정
        m_skyVertexBufferView.BufferLocation = m_skyVertexBuffer->GetGPUVirtualAddress();
        m_skyVertexBufferView.StrideInBytes = sizeof(SkyVertex);
        m_skyVertexBufferView.SizeInBytes = vertexBufferSize;

    }
