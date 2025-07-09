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
#include <unordered_map>
#include "WICTextureLoader12.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Transform.h"


using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DX;
using index_t = uint16_t;

class RenderManager
{
public:	
	RenderManager();
	RenderManager(ID3D12Device* device, CD3DX12_CPU_DESCRIPTOR_HANDLE& cvhand, UINT descriptorsize, int globaloffset, D3D12_VIEWPORT viewport, ComPtr<ID3D12PipelineState> pipelineState,
		ID3D12CommandQueue* commandQueue, IDXGISwapChain3* swapChain);//핸들 포인터..?
	~RenderManager();
	void Load(const wchar_t* fileName);
	Subset BuildMaterial(WaveFrontReader<index_t>::Material& cottageMaterial, int offset);
	Mainset BuildIndices(std::vector<WaveFrontReader<index_t>::Vertex> vertex, std::vector<index_t> vec);
	UINT CalcConstantBufferByteSize(UINT byteSize);
	std::vector<Mainset>& getTree();
	D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView();
	int getOffset();
	Subset CreateTexture(WaveFrontReader<index_t>::Material& cottageMaterial, int offset);
	std::vector<Subset> getLeaf();

	BoundingBox BoundBox(std::vector<WaveFrontReader<index_t>::Vertex> cottageVertices, std::vector<index_t> cottageindices);

	void WaitForPreviousFrame();
	void SRT(std::vector<WaveFrontReader<index_t>::Vertex> &Vertices, Transform* name);
	void SRT(float ScaleX = 1.0f, float ScaleY = 1.0f, float ScaleZ = 1.0f,
		float RadianX = 0.0f,
		float RadianY = 0.0f,
		float RadianZ = 0.0f,
		float TransX = 0.0f,
		float TransY = 0.0f,
		float TransZ = 0.0f);
	void CreateVertex();
	void DynamicVertex(Transform* name);
	XMMATRIX MoveObject(float ScaleX = 1.0f, float ScaleY = 1.0f, float ScaleZ = 1.0f,
		float RadianX = 0.0f,
		float RadianY =0.0f,
		float RadianZ = 0.0f,
		float TransX = 0.0f,
		float TransY = 0.0f,
		float TransZ = 0.0f);

	std::vector<WaveFrontReader<index_t>::Vertex>& getCottageVertices();

	void RotateKey(float radian, int pathCount);
	void CreateDirectTexture(const wchar_t* fileName, int offset);
	BoundingBox getBoundingBox();

	void setCamera(Camera* cam);
	bool getIsLamp();
	void setfAlpha0();
	void setfAlpha1();

	const wchar_t* getFileName();
	XMMATRIX GetIdentitySRTMatrix();
	std::vector<  std::vector<Vec3>> stairYVertex(const wchar_t* fileName);
	void minmaxBase(const wchar_t* fileName);
	std::vector<MinMax> getBasement();

private:
	ComPtr<ID3D12Device> m_device;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle;
	UINT mCbvSrvDescriptorSize;
	std::unordered_map<uint32_t, std::vector<index_t>> materialToIndices;
	std::vector<Subset> leaf;
	std::vector <Mainset> tree;
	UINT materialBufferSize;
	
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	int handleoffset;
	int handleoffset2;
	int handleoffset3;

	D3D12_VIEWPORT m_viewport;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	
	ComPtr<ID3D12Resource> textureUploadHeap;

	ID3D12CommandQueue* m_commandQueue;
	IDXGISwapChain3* m_swapChain;
	UINT64 m_fenceValue;
	ComPtr<ID3D12Fence> m_fence;
	HANDLE m_fenceEvent;
	UINT m_frameIndex;

	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandAllocator> commandAllocaotor;

	std::vector<WaveFrontReader<index_t>::Vertex> cottageVertices;
	std::vector<WaveFrontReader<index_t>::Vertex> RotatingcottageVertices;
	std::vector<index_t> cottageindices;
	std::vector<uint32_t> cottageAtrribute;
	std::vector<WaveFrontReader<index_t>::Material> cottageMaterial;

	std::vector<ComPtr<ID3D12Resource>> m_textureResources;
	//Transform& trans;
	Camera* m_cam;

	bool isLamp = false;

	WaveFrontReader<index_t> reader;

	const wchar_t* filename;
	std::vector<MinMax> baseLimitY;
	
};

