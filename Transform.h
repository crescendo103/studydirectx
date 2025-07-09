#pragma once

#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <DirectXCollision.h>
#include "typedef.h"
#include "WaveFrontReader.h"
#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <string>
#include "PathFinder.h"
#include <stack> 
#include <algorithm>   // std::clamp
#include <cmath>       // std::acos, std::atan2f 등


using namespace DirectX;
using namespace Microsoft::WRL;
using namespace DX;
using index_t = uint16_t;

enum class CurrentState {
	Xminus,//왼
	Zminus,//뒤
	Xplus,//오른
	Zplus//앞
};

class Camera;

class Transform
{
public:

	/*
	// static 함수로 선언
	static Transform& GetInstance() {
		if (instance == nullptr) { // nullptr 사용 권장
			instance = new Transform();
		}
		return *instance;
	}
	*/

	void calSRT(std::vector <Mainset>& Tree);
	XMMATRIX getMATRIX();
	void setBoundingBoxs(std::vector<BoundingBox> box);
	//void setFacmans(std::vector<RenderManager> pacman);
	void TranslateVertices(std::vector<WaveFrontReader<index_t>::Vertex>& vertices, float transY);
	int rollthedice();
	void setName(std::string name);
	std::string getName();
	void finishedRotate(bool isturnning);
	void defineCurrent();
	void finishedTrans(bool isfinished);

	void setCampointer(Camera* cam);
	void ComeOnMe(std::vector <Mainset>& Tree);

	void setRange(std::vector <Mainset>& Tree);
	BoundingBox getRange();

	void setComingstate(bool state);
	Camera* getcampointer();

	void getBack(std::vector <Mainset>& Tree);
	void setMapBox(std::vector<std::reference_wrapper<BoundingBox>> box);
	bool CheckCollision(const BoundingBox& checkbox, const XMFLOAT3& newPosition);
	XMMATRIX getRotateMatrix();
	bool getIsRange();
	XMMATRIX getbase();
	void getBackOnce();

public:
	//static Transform* instance; // 포인터로 변경
	Transform() {
		srand(time(nullptr));
		crashXfront = false;
		crashXback = false;
		crashZfront = false;
		crashZback = false;
		path = rollthedice();
		states.push_back(CurrentState::Xminus);
		states.push_back(CurrentState::Xplus);
		states.push_back(CurrentState::Zminus);
		states.push_back(CurrentState::Zplus);

	} // private 생성자
	// 소멸자는 private으로 만들고, 동적 할당된 인스턴스를 해제할 수 있도록 한다.
	~Transform() {}

	float ScaleX = 1.0f;
	float ScaleY = 1.0f;
	float ScaleZ = 1.0f;

	float Radian = 0.0f;

	float TransX = 0.0f;
	float TransY = 0.0f;
	float TransZ = 0.0f;
	XMMATRIX worldMatrix;

	std::vector<BoundingBox> Boxs;
	BoundingBox currentpacman;
	BoundingBox range;

	bool crashXfront;
	bool crashXback;
	bool crashZfront;
	bool crashZback;

	int path;
	int previouspath;

	std::vector<CurrentState> states;

	std::string m_Name;
	bool rotatedone;
	Point current;
	Point destination;

	CurrentState currentstate;
	CurrentState previousstate;

	bool istransingdone = true;
	float Speed = 0.1f;

	Camera* m_cam;
	XMVECTOR objectPos = { 18.0f, 0.0f, -19.0f, 1.0f };

	bool comingstate = false;

	XMVECTOR changeOccurePoint;

	bool walking = true;
	

	std::stack<XMFLOAT3> footprint;
	std::vector<std::reference_wrapper<BoundingBox>> otherBox;
	BoundingBox Extents;
	bool collisionDetected;

	bool isXplus;
	bool isZplus;

	bool collisionDetectedX;

	bool isSteped=false;
	XMVECTOR stepvec;

	XMMATRIX rotationMatrix;

	bool israge = false;

	XMMATRIX backTobase;

	XMMATRIX scaleMat;
	XMMATRIX rotateMat;
	XMMATRIX transMat;
	float prevYaw = 0.0f;
	bool test = false;
	XMVECTOR modelForward = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	
	bool firstGetBack = true;
	bool bArrivedAtCheckpoint = true;

	XMVECTOR currentDirection;//길 가던 방향
	XMVECTOR fromawayPlayerDir;//플레이어 반대방향
	XMVECTOR accumulateRotate;//방향 누적해서 현재 방향 계산
};

