#pragma once
#include <vector>
#include <wtypes.h>
#include <wrl/client.h>       // Microsoft::WRL::ComPtr
#include <dxgi1_4.h>          // IDXGISwapChain3

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


using namespace Microsoft::WRL;


class SceneManager
{
public:
	static SceneManager& GetInstance() {
		if (instance == nullptr) { // nullptr 사용 권장
			instance = new SceneManager();
		}
		return *instance;
	}

	void loadScene(HWND hwnd, RECT rect);
	void DrawScene(int currentScene);
	void setState(int scenenumber);
	int getCurrentScene();
	void setswapchain(ComPtr<IDXGISwapChain3> swapChain);
	std::vector<HBITMAP> getmaps();
	RECT getrect();

private:
	static SceneManager* instance;
	SceneManager() {}
	~SceneManager() {}
	std::vector<HBITMAP> hBitmaps;
	RECT clientRect;
	HWND m_hwnd;
	int currentScene = 0;
	ComPtr<IDXGISwapChain3> m_swapChain;
};

