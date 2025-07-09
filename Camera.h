#pragma once

#include <windows.h>
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
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include "fps.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "GemUI.h"

using namespace DirectX; 

class Camera
{
public:
	struct ObjectConstants {
		DirectX::XMFLOAT4X4 WorldViewProj = XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

	};//이거 카메라로 옮기자.

public:
	Camera(fps* ff, std::vector<std::reference_wrapper<BoundingBox>> Boxs, std::vector<BoundingBox> keyDoor, float width, float height);
	~Camera();
	void SetLens(float fovY, float aspect, float zn, float zf);
	void Strafe(float d);
	void Walk(float d);
	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	void UICamera(float offsetX, float offsetY, float uiWidth, float uiHeight, float NZ, float FZ);
	XMFLOAT4X4 GetProj4x4f();
	XMMATRIX GetView();
	XMMATRIX GetProj();


	void UpdateViewMatrix();

	void CameraUpdate(float deltaX, float deltaY);
	ObjectConstants getwoldViewProj();
	ObjectConstants getUIwoldViewProj();

	void UpdateBoundingBox();
	bool CheckCollision(const BoundingBox& otherBox, const XMFLOAT3& newPosition);

	void UpdateViewMatrix(float offsetX, float offsetY);
	void UpdateUIprojectionMatrix(float width, float height, float nearZ, float farZ);
	

	void setHimhitbox(BoundingBox him);
	XMVECTOR getMyPosition();
	XMFLOAT3 getMyPos();
	
	void setRange(BoundingBox range);
	bool isrange();
	XMFLOAT3 getmyWorld();
	void calculateSlope();
	XMFLOAT3 calculateCamYpos(XMFLOAT3 myPos);
	void setBasement(std::vector<MinMax> base);
	bool getGameOver();

private:
	std::vector<RenderManager> master2;

	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3 mPosition = { 0.0f, 3.0f, 10.0f };
	XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };
	// Cache frustum properties.
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	// 화면 크기 및 UI 요소 크기 정의 (하드코딩)
	float screenWidth = 1200.0f;  // 화면 너비
	float screenHeight = 900.0f;  // 화면 높이

	float uiWidth = 200.0f;  // UI 사각형의 너비
	float uiHeight = 100.0f; // UI 사각형의 높이

	// UI의 오른쪽 하단 위치 계산 (하드코딩)
	float xPosition = (1200.0f / 2.0f) - (200.0f / 2.0f); // 오른쪽 하단 기준 x 좌표
	float yPosition = -(900.0f / 2.0f) + (100.0f / 2.0f); // 아래쪽 기준 y 좌표
	const float GRAVITY = -0.009f;
	const float JUMP_VELOCITY = 0.39f;
	float velocity = 0.0f;
	bool isJumping = false;

	bool change = true;
	fps* myclock;
	ObjectConstants objConstants;
	ObjectConstants UI;

	XMFLOAT4X4 mView = objConstants.WorldViewProj;
	XMFLOAT4X4 mProj = objConstants.WorldViewProj;
	XMFLOAT4X4 mWorld = objConstants.WorldViewProj;

	XMFLOAT4X4 UIView = UI.WorldViewProj;
	XMFLOAT4X4 UIProj = UI.WorldViewProj;
	XMFLOAT4X4 UIWorld = UI.WorldViewProj;

	BoundingBox cameraBoundingBox;
	std::vector<std::reference_wrapper<BoundingBox>> otherBox;
	std::vector<BoundingBox> keyDoorBox;
	BoundingBox Him;
	BoundingBox Range;

	bool isranged = false;
	std::vector< std::vector<Vec3>> stairYPos;
	std::vector<MinMax> boundingFace;
	std::vector<MinMax> basement;

	bool shouldGameOver = false;
};
