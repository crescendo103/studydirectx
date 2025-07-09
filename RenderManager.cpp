#include "RenderManager.h"

RenderManager::RenderManager()
{
}

RenderManager::RenderManager(ID3D12Device* device, CD3DX12_CPU_DESCRIPTOR_HANDLE& cvhand, UINT descriptorsize, int globaloffset, D3D12_VIEWPORT viewport, ComPtr<ID3D12PipelineState> pipelineState,
    ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapChain)
{
	m_device = device;
    cbvHandle = cvhand;
    mCbvSrvDescriptorSize = descriptorsize;
    handleoffset = globaloffset;
    handleoffset2 = globaloffset;
    handleoffset3 = globaloffset;
    m_viewport = viewport;
    m_pipelineState = pipelineState;
    m_commandQueue = commandQueue;
    m_swapChain = swapChain;

    // Fence ����
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    // �̺�Ʈ �ڵ� ����
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    // �ʱ� ����ȭ ȣ��
    WaitForPreviousFrame();

    srand((unsigned int)time(NULL));
    //trans = Transform::GetInstance();

}

RenderManager::~RenderManager()
{
}

void RenderManager::Load(const wchar_t* fileName)
{
    filename = fileName;
    //WaveFrontReader<index_t> reader;
    HRESULT hr = reader.Load(fileName);
    cottageVertices = reader.getVertices();
    RotatingcottageVertices = cottageVertices;//ȸ���� ����
    cottageindices = reader.getindices();
    cottageAtrribute = reader.getAttributes();
    cottageMaterial = reader.getMaterial();

    wchar_t strName[MAX_PATH];
    wchar_t strTexture[MAX_PATH];
    wchar_t strNormalTexture[MAX_PATH];
    wchar_t strSpecularTexture[MAX_PATH];
    wchar_t strEmissiveTexture[MAX_PATH];
    wchar_t strRMATexture[MAX_PATH];
    
    for (int i = 0; i < cottageMaterial.size(); i++) {
        cottageMaterial[i].strName;
        cottageMaterial[i].strTexture;
    }

    //�Ⱥ��̴� ������ �̵���Ű�� ���� �ڵ��
    if(wcscmp(fileName, L"asset\\stuff\\lantern-textured-low-l-eevee-retexture.obj") == 0){
        XMMATRIX worldMatrix = MoveObject(1.3f, 1.3f, 1.3f, 0.0f, 0.0f, 10.0f, 0.0f, -0.50f, 0.0f);
        for (int i = 0; i < cottageVertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, worldMatrix);
            XMStoreFloat3(&cottageVertices[i].position, pos);
        }
        CreateVertex();
        isLamp = true;
    }else if (wcscmp(fileName, L"asset\\stuff\\fishh.obj") == 0 || wcscmp(fileName, L"asset\\stuff\\fish1.obj") == 0 || wcscmp(fileName, L"asset\\stuff\\non.obj") == 0 || wcscmp(fileName, L"asset\\stuff\\gamestart.obj") == 0
        || wcscmp(fileName, L"asset\\stuff\\gameend.obj") == 0 || wcscmp(fileName, L"asset\\stuff\\Skybox.obj") == 0){//fileName == L"Skybox.obj") {
        XMMATRIX worldMatrix = MoveObject(0.05f, 0.05f, 0.05f, 90.0f, 90.0f, 0.0f, 20.0f, 110.0f, -5.0f);

        for (int i = 0; i < cottageVertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, worldMatrix);
            XMStoreFloat3(&cottageVertices[i].position, pos);
        }

        CreateVertex();
    }
    else if (std::wstring(fileName).find(L"janitor") != std::wstring::npos) {
        XMMATRIX worldMatrix = MoveObject(5.0f, 5.0f, 5.0f, 0.0f, 90.0f, 0.0f, 18.0f, 0.0f, -19.0f);       
        for (int i = 0; i < cottageVertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, worldMatrix);
            XMStoreFloat3(&cottageVertices[i].position, pos);
        }        
    }
    else if (fileName == L"asset\\gsem\\compactgem.obj") {
        XMMATRIX worldMatrix = MoveObject(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f);

        for (int i = 0; i < cottageVertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, worldMatrix);
            XMStoreFloat3(&cottageVertices[i].position, pos);
        }
        CreateVertex();
        
    }
    else //default����
    {
        CreateVertex();
    }
   
   
   

    for (size_t i = 0; i < cottageAtrribute.size(); ++i) {
        uint32_t materialIndex = cottageAtrribute[i];
        materialToIndices[materialIndex].push_back(cottageindices[i * 3 + 0]);
        materialToIndices[materialIndex].push_back(cottageindices[i * 3 + 1]);
        materialToIndices[materialIndex].push_back(cottageindices[i * 3 + 2]);
    }

    materialToIndices;
    materialBufferSize = static_cast<UINT>(cottageMaterial.size() * sizeof(WaveFrontReader<index_t>::Material));
    //���������� ������
    if (cottageMaterial.size() == 1) {
        int i = 0;
        if (cottageMaterial[i].strTexture[0] != L'\0') {
            //Todo
            cottageMaterial[i].bUseTexture = 1;
            Subset sb;
            sb = BuildMaterial(cottageMaterial[i], handleoffset);
            handleoffset++;
            CreateTexture(cottageMaterial[i], handleoffset);
            sb.istexture = true;
            leaf.push_back(sb);
            handleoffset++;
        }
        else {
            Subset temp = BuildMaterial(cottageMaterial[i], handleoffset);
            leaf.push_back(temp);
            handleoffset++;
        }


        
            Mainset tmp = BuildIndices(cottageVertices, materialToIndices[0]);
            tree.push_back(tmp);
        

    }
    else {
        //���������� ������
        for (int i = 1; i < cottageMaterial.size(); i++)
        {

            if (cottageMaterial[i].strTexture[0] != L'\0') {
                //Todo
                cottageMaterial[i].bUseTexture = 1;
                Subset sb;
                sb = BuildMaterial(cottageMaterial[i], handleoffset);
                handleoffset++;
                CreateTexture(cottageMaterial[i], handleoffset);
                sb.istexture = true;
                leaf.push_back(sb);
                handleoffset++;
            }
            else {
                Subset temp = BuildMaterial(cottageMaterial[i], handleoffset);
                leaf.push_back(temp);
                handleoffset++;
            }
        }

        for (int i = 1; i <= materialToIndices.size(); i++) {
            Mainset tmp = BuildIndices(cottageVertices,materialToIndices[i]);
            tree.push_back(tmp);
        }
    }
}

Subset RenderManager::BuildMaterial(WaveFrontReader<index_t>::Material& cottageMaterial, int offset)
{
    //���������� ����
    Subset subset;
    subset.offset = offset;
    UINT matBufferSize = sizeof(WaveFrontReader<index_t>::Material);
    UINT alignedSize = CalcConstantBufferByteSize(sizeof(WaveFrontReader<index_t>::Material));
   
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize);

    m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&subset.materialBuffer));

    // ������ ����
    void* mappedData;
    subset.materialBuffer->Map(0, nullptr, &mappedData);

    // ������ ���� ���� �� ���  �ʿ����
    WaveFrontReader<index_t>::Material* debugMaterial = reinterpret_cast<WaveFrontReader<index_t>::Material*>(mappedData);

    memcpy(mappedData, &cottageMaterial, matBufferSize);//cottageMaterial.data(), materialBufferSize);
    subset.materialBuffer->Unmap(0, nullptr);

    // GPU ���� �ּ� ����
    subset.materialBufferAddress = subset.materialBuffer->GetGPUVirtualAddress();
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbv2Handle;

    cbv2Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHandle, offset, mCbvSrvDescriptorSize);//change offset+1

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = subset.materialBufferAddress;
    cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(WaveFrontReader<index_t>::Material));

    m_device->CreateConstantBufferView(
        &cbvDesc,
        cbv2Handle);

    

    return subset;
}

Mainset RenderManager::BuildIndices(std::vector<WaveFrontReader<index_t>::Vertex> vertex ,std::vector<index_t> vec)
{
    Mainset mainset;

    UINT indexcount = vec.size();//IndicesVector.size();
    mainset.indicesCount = indexcount;
    UINT indexBufferSize = sizeof(index_t) * (vec.size());

    // �ε��� ���� ����              
    CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
    m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &indexBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mainset.indicesBuffer)
    );

    // �ε��� �����͸� ���ε�
    void* pIndexDataBegin;
    mainset.indicesBuffer->Map(0, nullptr, &pIndexDataBegin);
    memcpy(pIndexDataBegin, vec.data(), indexBufferSize);//.data(), ISize);//sizeof(indices));
    mainset.indicesBuffer->Unmap(0, nullptr);

    // �ε��� ���� �� ����
    mainset.indicesBufferView.BufferLocation = mainset.indicesBuffer->GetGPUVirtualAddress();  // �ε��� ������ GPU ���� �ּ�
    mainset.indicesBufferView.SizeInBytes = indexBufferSize;//sizeof(indices);  // �ε��� ������ ũ��
    mainset.indicesBufferView.Format = DXGI_FORMAT_R16_UINT;  // �ε��� �������� ���� (16��Ʈ unsigned int)

    BoundingBox thebox = BoundBox(vertex ,vec);
    mainset.Box = thebox;
    return mainset;
}

UINT RenderManager::CalcConstantBufferByteSize(UINT byteSize)
{
    // 256����Ʈ�� ����� ����
    return (byteSize + 255) & ~255;
}

std::vector<Mainset>& RenderManager::getTree()
{
    return tree;
}

D3D12_VERTEX_BUFFER_VIEW& RenderManager::getVertexBufferView()
{
    return m_vertexBufferView;
}

int RenderManager::getOffset()
{
    return handleoffset;
}


Subset RenderManager::CreateTexture(WaveFrontReader<index_t>::Material& cottageMaterial, int offset)
{

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocaotor)));//�ʱ�ȭ���� ���� �Ҵ���
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocaotor.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&commandList)));//�ʱ�ȭ ����Ʈ

    Subset subset;
    subset.istexture = true;

    ComPtr<ID3D12Resource> m_pTex2D;

    //���ҽ� ����
    D3D12_RESOURCE_DESC    m_tDesc;
    m_tDesc.MipLevels = 1;
    m_tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    m_tDesc.Width = m_viewport.Width;        // 1200
    m_tDesc.Height = m_viewport.Height;        // 900
    m_tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    m_tDesc.DepthOrArraySize = 1;
    m_tDesc.SampleDesc.Count = 1;
    m_tDesc.SampleDesc.Quality = 0;
    m_tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    m_tDesc.Layout = D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN;
    m_tDesc.Alignment = 0; // �⺻ ���� ���

    HRESULT hr = m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &m_tDesc,
        D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_pTex2D));

    //�ϴ� �޾Ƶ���
    std::unique_ptr<uint8_t[]> decodedData;    // ���ڵ��� ������ ������
    D3D12_SUBRESOURCE_DATA subresourceData;    // ���� ���ҽ� ������
    //��α׿����� ���ͷ� ����
    wchar_t* fileName = cottageMaterial.strTexture;//L"Cube.obj";
    hr = LoadWICTextureFromFile(m_device.Get(), fileName , &m_pTex2D, decodedData, subresourceData);// L"gray.png"
    if (!SUCCEEDED(hr)) {
        assert("�̹��� �ε����");
    }
    const UINT64 uploadBufferSize =
        GetRequiredIntermediateSize(m_pTex2D.Get(),
            0, static_cast<unsigned int>(1));//���ϼ��긮�ҽ�


    hr = m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

    if (FAILED(hr))
        assert(nullptr);

    UpdateSubresources(commandList.Get(),//�ʿ����...?
        m_pTex2D.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(1),//(vecSubresources.size()),���ϼ��긮�ҽ�
        &subresourceData);

    // SRV ���� �� ���ҽ� ���¸� �����մϴ�.
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_pTex2D.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST; // ���� ����
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // ������ ����
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // ���ҽ� �庮 �߰�
    commandList->ResourceBarrier(1, &barrier);


    commandList->Close();
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForPreviousFrame();
    commandAllocaotor->Reset();
    commandList->Reset(commandAllocaotor.Get(), m_pipelineState.Get());



    // SRV ����
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_pTex2D->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHandle, offset, mCbvSrvDescriptorSize);

    m_device->CreateShaderResourceView(m_pTex2D.Get(), &srvDesc, srvHandle);
    
    m_textureResources.push_back(m_pTex2D);
    return subset;
}

std::vector<Subset> RenderManager::getLeaf()
{
    return leaf;
}

BoundingBox RenderManager::BoundBox(std::vector<WaveFrontReader<index_t>::Vertex> cottageVertices, std::vector<index_t> cottageindices)
{
    XMFLOAT3 minPos(FLT_MAX, FLT_MAX, FLT_MAX); // �ּҰ�
    XMFLOAT3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX); // �ִ밪

    // �������� �ּҰ�/�ִ밪�� ���
    for (int i = 0; i < cottageindices.size(); i++) {
        //XMFLOAT3 vertexPos = cottageVertices[i].position;
        XMFLOAT3 vertexPos = cottageVertices[cottageindices[i]].position;

        minPos.x = std::min(minPos.x, vertexPos.x);
        minPos.y = std::min(minPos.y, vertexPos.y);
        minPos.z = std::min(minPos.z, vertexPos.z);

        maxPos.x = std::max(maxPos.x, vertexPos.x);
        maxPos.y = std::max(maxPos.y, vertexPos.y);
        maxPos.z = std::max(maxPos.z, vertexPos.z);
    }
    XMFLOAT3 mainspot((minPos.x + maxPos.x) / 2, (minPos.y + maxPos.y) / 2, (minPos.z + maxPos.z) / 2); // �߾���
    XMFLOAT3 rangee((maxPos.x - minPos.x) / 2, (maxPos.y - minPos.y) / 2, (maxPos.z - minPos.z) / 2); // ������ ����
    // BoundingBox ��ü ����    
    BoundingBox Bounds;
    Bounds.Center = mainspot;  // �߽� ����
    Bounds.Extents = rangee; //���� ����

    return Bounds;
}

void RenderManager::WaitForPreviousFrame()
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

void RenderManager::SRT(std::vector<WaveFrontReader<index_t>::Vertex> &Vertices,Transform* name)
{
   
    
    //name->ComeOnMe(tree);
    name->calSRT(tree);
    name->finishedTrans(false)
        ;
    XMMATRIX worldMatrix;
    worldMatrix = name->getMATRIX();

    
        
        for (int i = 0; i < Vertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&Vertices[i].position);
            //XMVECTOR pos = XMLoadFloat3(&RotatingcottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, worldMatrix);
            XMStoreFloat3(&Vertices[i].position, pos);
        }
    
    
    
    //name->finishedTrans(true);
    
    //name->finishedRotate(true);

}

void RenderManager::SRT(float ScaleX, float ScaleY, float ScaleZ, float RadianX, float RadianY, float RadianZ, float TransX, float TransY, float TransZ)
{
    XMMATRIX worldMatrix = MoveObject(ScaleX, ScaleY, ScaleZ, RadianX, RadianY, RadianZ, TransX, TransY, TransZ);

    for (int i = 0; i < cottageVertices.size(); i++) {
        XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
        pos = XMVector3TransformCoord(pos, worldMatrix);
        XMStoreFloat3(&cottageVertices[i].position, pos);
    }
    CreateVertex();
}

void RenderManager::CreateVertex()
{
    const UINT vertexBufferSize = static_cast<UINT>(cottageVertices.size() * sizeof(Vertex));

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));

    // Copy the triangle data to the vertex buffer.
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, cottageVertices.data(), vertexBufferSize);
    m_vertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;

}

void RenderManager::DynamicVertex(Transform* name)
{
    SRT(cottageVertices, name);
    CreateVertex();
}

XMMATRIX RenderManager::MoveObject(float ScaleX, float ScaleY, float ScaleZ, float RadianX, float RadianY, float RadianZ, float TransX, float TransY, float TransZ)
{
    XMMATRIX scaleMatrix = XMMatrixScaling(ScaleX, ScaleY, ScaleZ);

    // �� �࿡ ���� ȸ�� ��ȯ ���
    XMMATRIX rotationXMatrix = XMMatrixRotationX(XMConvertToRadians(RadianX));
    XMMATRIX rotationYMatrix = XMMatrixRotationY(XMConvertToRadians(RadianY));
    XMMATRIX rotationZMatrix = XMMatrixRotationZ(XMConvertToRadians(RadianZ));

    // ȸ�� ��� ���� (���� �߿�! �Ϲ������� Z -> X -> Y ���� ���)
    XMMATRIX rotationMatrix = rotationZMatrix * rotationXMatrix * rotationYMatrix;

    // X�� �������� 5��ŭ �̵��ϴ� ��ȯ ���            
    XMMATRIX translationMatrix = XMMatrixTranslation(TransX, TransY, TransZ);

    XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    return worldMatrix;
}

std::vector<WaveFrontReader<index_t>::Vertex>& RenderManager::getCottageVertices()
{
    // TODO: ���⿡ return ���� �����մϴ�.
    return cottageVertices;
}

void RenderManager::RotateKey(float radian, int pathCount)
{
    if (pathCount == 60) {
        XMMATRIX world = MoveObject(1.0f, 1.0f, 1.0f, 0.0f, radian);

        //std::vector<WaveFrontReader<index_t>::Vertex>& keyy = key.getCottageVertices();
        for (int i = 0; i < cottageVertices.size(); i++) {
            XMVECTOR pos = XMLoadFloat3(&cottageVertices[i].position);
            pos = XMVector3TransformCoord(pos, world);
            XMStoreFloat3(&cottageVertices[i].position, pos);
        }
        CreateVertex();
    }
}

void RenderManager::CreateDirectTexture(const wchar_t* fileName, int offset)
{
    WaveFrontReader<index_t>::Material cottageMaterial;
    //cottageMaterial.strTexture = fileName;
    wcscpy_s(cottageMaterial.strTexture, fileName);
    CreateTexture(cottageMaterial, offset);
    //BuildMaterial(cottageMaterial, offset);
}

BoundingBox RenderManager::getBoundingBox()
{
    // TODO: ���⿡ return ���� �����մϴ�.
    BoundingBox once = tree[0].Box;
    return once;
}

void RenderManager::setCamera(Camera* cam)
{
    m_cam = cam;
}

bool RenderManager::getIsLamp()
{
    return isLamp;
}

void RenderManager::setfAlpha0()
{
    //���������� ������
    for (int i = 1; i < cottageMaterial.size(); i++)
    {

        if (cottageMaterial[i].strTexture[0] != L'\0') {
            //Todo
            cottageMaterial[i].bUseTexture = 1;
            cottageMaterial[i].fAlpha = 0.0f;//�߰�
            Subset sb;
            sb = BuildMaterial(cottageMaterial[i], handleoffset2);
            handleoffset2++;
            CreateTexture(cottageMaterial[i], handleoffset2);
            sb.istexture = true;
            leaf.push_back(sb);
            handleoffset2++;
        }
        else {
            cottageMaterial[i].fAlpha = 0.0f;//�߰�
            Subset temp = BuildMaterial(cottageMaterial[i], handleoffset2);
            leaf.push_back(temp);
            handleoffset2++;
        }
    }
}

void RenderManager::setfAlpha1()
{
    //���������� ������
    for (int i = 1; i < cottageMaterial.size(); i++)
    {

        if (cottageMaterial[i].strTexture[0] != L'\0') {
            //Todo
            cottageMaterial[i].bUseTexture = 1;
            cottageMaterial[i].fAlpha = 1.0f;//�߰�
            Subset sb;
            sb = BuildMaterial(cottageMaterial[i], handleoffset3);
            handleoffset3++;
            CreateTexture(cottageMaterial[i], handleoffset3);
            sb.istexture = true;
            leaf.push_back(sb);
            handleoffset3++;
        }
        else {
            cottageMaterial[i].fAlpha = 1.0f;//�߰�
            Subset temp = BuildMaterial(cottageMaterial[i], handleoffset3);
            leaf.push_back(temp);
            handleoffset3++;
        }
    }
}

const wchar_t* RenderManager::getFileName()
{
    return filename;
}

XMMATRIX RenderManager::GetIdentitySRTMatrix()
{
    XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
    XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    XMMATRIX srt = scale * rotation * translation;

    return srt;
}

//��ü �������п��� ����� y�࿡ �����Ѱ��
//��� �Ľ�
std::vector<  std::vector<Vec3>> RenderManager::stairYVertex(const wchar_t* fileName)
{
    float Diff = 0.1f;//������

    std::ifstream infile(fileName);
    std::string line;

    std::vector<Vec3> vertices;  // ���� ��ǥ ���� (1-based index)
    std::vector<Vec3> normals;   // ���� ���� (1-based index)
    std::vector<int> y_like_normal_indices;  // y��� ���� ������ ���� �ε���
    std::vector< std::vector<Vec3>> face_vertex_coords;  // (face line ��ȣ, ���� ��ǥ)

    int face_line = 0;
    int line_number = 0;

    while (std::getline(infile, line)) {
        line_number++;
        std::istringstream iss(line);

        if (line.rfind("v ", 0) == 0) {
            // ���� ó�� (v)
            std::string dummy;
            float x, y, z;
            iss >> dummy >> x >> y >> z;
            vertices.push_back({ x, y, z });
        }
        else if (line.rfind("vn", 0) == 0) {
            // ���� ó�� (vn)
            std::string dummy;
            float x, y, z;
            iss >> dummy >> x >> y >> z;
            normals.push_back({ x, y, z });
            int vn_index = normals.size();

            //if (std::abs(x) < Diff && std::abs(y - 1.0f) < Diff && std::abs(z) < Diff) {
                y_like_normal_indices.push_back(vn_index);
            //}
        }
        else if (line.rfind("f", 0) == 0) {
            // face ó�� (f)
            face_line++;

            std::string dummy, vertex;
            iss >> dummy;
            std::vector<Vec3> facevertex;

            while (iss >> vertex) {
                std::stringstream vss(vertex);
                std::string v_str, vt_str, vn_str;
                std::getline(vss, v_str, '/');
                std::getline(vss, vt_str, '/');
                std::getline(vss, vn_str, '/');
                

                if (!vn_str.empty()) {
                    int vn_idx = std::stoi(vn_str);
                    // ���� �ε����� y��� ���� ������ ���� ����Ʈ�� �ִ� ��츸 ó��
                    if (std::find(y_like_normal_indices.begin(), y_like_normal_indices.end(), vn_idx) != y_like_normal_indices.end()) {
                        int v_idx = std::stoi(v_str);  // ���� �ε��� 1���� ����
                        if (v_idx > 0 && v_idx <= (int)vertices.size()) {
                            Vec3 vertex_pos = vertices[v_idx - 1];
                            facevertex.push_back(vertex_pos);                            
                        }
                    }
                }
            }
            if (facevertex.size() != 0) {
                face_vertex_coords.push_back(facevertex);
                facevertex.clear();
            }
        }
    }
    return face_vertex_coords;
    // ���� face_vertex_coords �ȿ� ���ϴ� face_line�� ���� ��ǥ���� �����
}

void RenderManager::minmaxBase(const wchar_t* fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "������ �� �� �����ϴ�." << std::endl;
    }

    float minX = 1000.0f;
    float maxX = -1000.0f;
    float minZ = 1000.0f;
    float maxZ = -1000.0f;
    float firstY = 0.0f;
    bool firstYFound = false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("v ", 0) == 0) { // "v "�� �����ϴ� �ٸ� ó��
            std::istringstream iss(line);
            std::string v;
            float x, y, z;
            iss >> v >> x >> y >> z;

            if (!firstYFound) {//ù��° y���Ľ�
                firstY = y;
                firstYFound = true;
            }

            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (z < minZ) minZ = z;
            if (z > maxZ) maxZ = z;
        }
    }
    file.close();
    MinMax temp;
    temp.anyYpos = firstY;
    temp.maxX = maxX;
    temp.minX = minX;
    temp.maxZ = maxZ;
    temp.minZ = minZ;

    baseLimitY.push_back(temp);

    //���������� �ٴ� �������� �ϳ����� ���� �Ľ������ʰ� ��������
    temp.maxX = 2.14f;
    temp.minX = -2.6f;
    temp.maxZ = 9.61f;
    temp.minZ = 3.28f;
    temp.anyYpos = 0.046f;
    baseLimitY.push_back(temp);
   
}

std::vector<MinMax> RenderManager::getBasement()
{
    return baseLimitY;
}
