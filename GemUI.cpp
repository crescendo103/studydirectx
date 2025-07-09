#include "GemUI.h"


GemUI* GemUI::instance = nullptr;

void GemUI::setPlayerPos(XMFLOAT3 mPos)
{
	m_Pos = mPos;
}

//맵에 3개의 보석 위치
//shouldUI는 화살표 UI를 렌더할지 말지 결정하는 bool
void GemUI::initGem()
{
	gem1 = true;
	gem2 = true;
	gem3 = true;
	shouldUI = false;
	gemcount = 0;
}

void GemUI::updateGem()
{	
	//플레이어가 지정된 좌표안에 있을경우 보석위에 화살표가 표시됨
	if (m_Pos.x > -10.5f && m_Pos.x < -4.5f && m_Pos.z < -28.0f && m_Pos.z > -34.0f) {		
		shouldUI = true;
		gem1range = true;

		if (gem1 == false) {
			shouldUI = false;
			gem1range = false;
		}	
		
	}else	if (m_Pos.x > -4.0f && m_Pos.x < 1.6f && m_Pos.z < -16.0f && m_Pos.z > -34.0f) {		
		shouldUI = true;
		gem2range = true;

		if (gem2 == false) {
			shouldUI = false;
			gem2range = false;
		}

	}else if (m_Pos.x > 2.0f && m_Pos.x < 8.0f && m_Pos.z < -10.0f && m_Pos.z > -20.0f) {
		
		shouldUI = true;
		gem3range = true;

		if (gem3 == false) {
			shouldUI = false;
			gem3range = false;
		}
	}
	else {
		gem1range = false;
		gem2range = false;
		gem3range = false;
	}

}

//보석위의 빨간화살표를 그릴 스크린 좌표 계산
XMFLOAT2 GemUI::WorldToScreen(const XMFLOAT3& worldPos, const XMMATRIX& viewProj, float screenWidth, float screenHeight)
{
	XMVECTOR worldVec = XMLoadFloat3(&worldPos);

	// 월드 -> 클립 좌표 변환
	XMVECTOR clipPos = XMVector3Transform(worldVec, viewProj);

	// NDC 좌표로 변환 (Clip Space -> NDC)
	clipPos = XMVectorScale(clipPos, 1.0f / XMVectorGetW(clipPos));

	// 스크린 좌표로 변환 (NDC -> Screen Space)
	float x = (XMVectorGetX(clipPos) + 1.0f) * 0.5f * screenWidth;
	float y = (1.0f - XMVectorGetY(clipPos)) * 0.5f * screenHeight; // y축 뒤집기

	return XMFLOAT2(x, y);
}

bool GemUI::getGem1State()
{
	return gem1range;
}

bool GemUI::getGem2State()
{
	return gem2range;
}

bool GemUI::getGem3State()
{
	return gem3range;
}

void GemUI::setGem1State(bool state)
{
	gem1 = state;
}

void GemUI::setGem2State(bool state)
{
	gem2 = state;
}

void GemUI::setGem3State(bool state)
{
	gem3 = state;
}

bool GemUI::getShouldUI()
{
	return shouldUI;
}

int GemUI::getgemCount()
{
	return gemcount;
}

void GemUI::setgemCount()
{
	gemcount++;
	if (gemcount > 3) {
		gemcount = 3;
	}
}
