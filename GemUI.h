#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class GemUI
{
public:
	static GemUI& GetInstance() {
		if (instance == nullptr) { // nullptr 사용 권장
			instance = new GemUI();
		}
		return *instance;
	}

	void setPlayerPos(XMFLOAT3 mPos);
	void initGem();
	void updateGem();
	XMFLOAT2 WorldToScreen(const XMFLOAT3& worldPos, const XMMATRIX& viewProj, float screenWidth, float screenHeight);
	bool getGem1State();
	bool getGem2State();
	bool getGem3State();
	void setGem1State(bool state);
	void setGem2State(bool state);
	void setGem3State(bool state);
	bool getShouldUI();
	int getgemCount();
	void setgemCount();

private:
	static GemUI* instance;

	XMFLOAT3 m_Pos;
	bool gem1 = false;
	bool gem2 = false;
	bool gem3 = false;
	bool shouldUI = false;
	int gemcount;
	bool gem1range = false;
	bool gem2range = false;
	bool gem3range = false;

};