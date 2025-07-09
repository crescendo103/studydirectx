#include "Camera.h"


Camera::Camera(fps* ff, std::vector<std::reference_wrapper<BoundingBox>> Boxs, std::vector<BoundingBox> keyDoor, float width, float height)
{
	myclock = ff;	
	float fovAngleY = XMConvertToRadians(45.0f);
	SetLens(fovAngleY, width / height, 1.0f, 1000.0f);
	otherBox = Boxs;//�� �浹ü
	keyDoorBox = keyDoor;

	RenderManager rm;
	stairYPos = rm.stairYVertex(L"asset\\stair\\stair.obj");
	calculateSlope();
	int test = 0;
	
}

Camera::~Camera()
{
	
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));

	change = true;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	change = true;
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	change = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	change = true;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	change = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	change = true;
}

//��������� ������� �ʰ� ī�޶� �ϳ� �� �ξ UI�� ������ �κ�
void Camera::UICamera(float offsetX, float offsetY, float uiWidth, float uiHeight, float NZ, float FZ)
{
	UpdateViewMatrix(offsetX, offsetY);
	UpdateUIprojectionMatrix(uiWidth, uiHeight, NZ, FZ);
	XMMATRIX view = XMLoadFloat4x4(&UIView);
	XMMATRIX proj = XMLoadFloat4x4(&UIProj); 
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX worldViewProj = view * proj * world; 
	XMStoreFloat4x4(&UI.WorldViewProj, XMMatrixTranspose(worldViewProj));
}

XMFLOAT4X4 Camera::GetProj4x4f()
{
	return mProj;
}

XMMATRIX Camera::GetView()
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj()
{
	return XMLoadFloat4x4(&mProj);
}

void Camera::UpdateViewMatrix()
{
	if (change)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&mPosition);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);
		
		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		//my
		// World * View * Proj ���
		XMMATRIX view = XMLoadFloat4x4(&mView);
		XMMATRIX proj = XMLoadFloat4x4(&mProj); // mProj�� ������ �����Ǿ� �־�� �մϴ�.
		XMMATRIX world = XMLoadFloat4x4(&mWorld);//????
		XMMATRIX worldViewProj = view * proj * world;  // World ����� Identity�� ����

		//XMStoreFloat4x4(&objConstants.WorldViewProj, worldViewProj);
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));

		change = false;
	}
}

void Camera::CameraUpdate(float deltaX, float deltaY)
{
	 const float dt = myclock->deltaTime;

	 XMFLOAT3 oldPosition = mPosition;  // ���� ��ġ ����
	 XMFLOAT3 newPosition = mPosition;  // ���ο� ��ġ ���
	 deltaX *= 0.005f;
	 deltaY *= 0.005f;

	 if (GetAsyncKeyState('W') & 0x8000) {
		 Walk(0.1101f);// *dt);
		 newPosition = mPosition;  // �̵� �� ��ġ ������Ʈ
		 mPosition = oldPosition;  // �ϴ� ���� ��ġ�� �ǵ�������
	 }
	 if (GetAsyncKeyState('S') & 0x8000) {
		 Walk(-0.1101f); //* dt);
		 newPosition = mPosition;  // �̵� �� ��ġ ������Ʈ
		 mPosition = oldPosition;  // �ϴ� ���� ��ġ�� �ǵ�������
	 }
	 if (GetAsyncKeyState('A') & 0x8000) {
		 Strafe(-0.1101f);// *dt);
		 newPosition = mPosition;  // �̵� �� ��ġ ������Ʈ
		 mPosition = oldPosition;  // �ϴ� ���� ��ġ�� �ǵ�������
	 }
	 if (GetAsyncKeyState('D') & 0x8000) {
		 Strafe(0.1101f);// *dt);
		 newPosition = mPosition;  // �̵� �� ��ġ ������Ʈ
		 mPosition = oldPosition;  // �ϴ� ���� ��ġ�� �ǵ�������
	 }

	 //6.04
	 mPosition;
	 XMFLOAT3 StairPos = calculateCamYpos(newPosition);
	 newPosition = StairPos;
	 mPosition = oldPosition;	 
	 RotateY(deltaX);
	 Pitch(deltaY);

	 //���� ���� ����
	 if (GetAsyncKeyState(VK_SPACE) & 0x8000 && isJumping==false) {
		 velocity = JUMP_VELOCITY;		
		 isJumping = true;
	 }

	 float ddt = myclock->deltaTime;
	 velocity += GRAVITY * ddt;  // �ӵ� ������Ʈ

	 velocity = -0.01f;
	 newPosition.y += velocity * ddt;// ��ġ ������Ʈ
	 
	 if (newPosition.y < 0.5f) {
		 newPosition.y = 0.5f;  //6.4		 
		 isJumping = false;
	 }
	 

	 change = true;
	bool collisionDetected = false;

	if (CheckCollision(Him, newPosition))
	{
		collisionDetected = true;		
		shouldGameOver = true;//���ӿ��� UI bool
	}

	if (CheckCollision(Range, newPosition))
	{		
		isranged = true;
	}
	else {
		isranged = false;
	}

	for (auto& objectBox : otherBox)//��,����� x
	{
		if (CheckCollision(objectBox.get(), newPosition))
		{
			collisionDetected = true;
			break;
		}
	}

	if (!collisionDetected)
	{
		mPosition=newPosition;  // �浹 ������ �̵� ���
	}
	else {
		mPosition = oldPosition;
	}

	GemUI& gem = GemUI::GetInstance();
	gem.setPlayerPos(mPosition);

	UpdateViewMatrix();
	UpdateBoundingBox();
	
}

Camera::ObjectConstants Camera::getwoldViewProj()
{
	return objConstants;
}

Camera::ObjectConstants Camera::getUIwoldViewProj()
{
	return UI;
}

void Camera::UpdateBoundingBox()
{
	cameraBoundingBox.Center = mPosition;
	cameraBoundingBox.Extents = XMFLOAT3(0.10f, 0.10f, 0.10f);
}

bool Camera::CheckCollision(const BoundingBox& checkbox, const XMFLOAT3& newPosition)
{
	// ���ο� ��ġ�� ī�޶��� �ٿ�� �ڽ��� �̵�
	BoundingBox newCameraBox;
	newCameraBox.Center = newPosition;
	newCameraBox.Extents = cameraBoundingBox.Extents;  // ũ��� �����ϰ� ����

	// ���ο� ��ġ���� �浹 ���� Ȯ��
	return newCameraBox.Intersects(checkbox);
}

void Camera::UpdateViewMatrix(float offsetX, float offsetY)
{
	// UI ��Ҹ� DirectX�� Ŭ�� ��ǥ�迡 ���߱� ���� Y���� ������
	XMMATRIX view = XMMatrixTranslation(offsetX, -offsetY, 0.0f); // ������ �ϴ����� �̵�
	XMStoreFloat4x4(&UIView, view);
}

void Camera::UpdateUIprojectionMatrix(float width, float height, float nearZ, float farZ)
{
	//XMMATRIX proj = XMMatrixOrthographicLH(width, height, nearZ, farZ);
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;

	// ���� �ϴ� (0,0)�� �ƴ϶�, �߾��� (0,0)�� �ǵ��� ����
	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, nearZ, farZ);
	XMStoreFloat4x4(&UIProj, proj);
}

void Camera::setHimhitbox(BoundingBox him)
{
	Him = him;
}

XMVECTOR Camera::getMyPosition()
{
	XMVECTOR playerVec = XMLoadFloat3(&mPosition);
	return playerVec;
}

XMFLOAT3 Camera::getMyPos()
{
	return mPosition;
}

void Camera::setRange(BoundingBox range)
{
	Range = range;
}

bool Camera::isrange()
{
	return isranged;
}

XMFLOAT3 Camera::getmyWorld()
{
	return mPosition;
}

//���� x,y2���� �����ڿ� �׶� y����ǥ ����
void Camera::calculateSlope()
{
	for (int i = 0; i < stairYPos.size(); i++) {
		MinMax minmax_face;
		for (int j = 0; j < stairYPos[j].size(); j++) {
			Vec3 pos = stairYPos[i][j];
			minmax_face.anyYpos = pos.y;//������ ������ y��

			if (pos.x > minmax_face.maxX) {
				minmax_face.maxX = pos.x;
			}
			if (pos.x < minmax_face.minX) {
				minmax_face.minX = pos.x;
			}
			if (pos.z > minmax_face.maxZ) {
				minmax_face.maxZ = pos.z;
			}
			if (pos.z < minmax_face.minZ) {
				minmax_face.minZ = pos.z;
			}
		}
		boundingFace.push_back(minmax_face);	
		
	}	
}

//��� ���� ���� �ö󰥶� ī�޶� y��ǥ ����
XMFLOAT3 Camera::calculateCamYpos(XMFLOAT3 myPos)
{
	//��ܿ� ��ġ�Ұ�� y��
	XMFLOAT3 val=myPos;
	for (int i = 0; i < boundingFace.size(); i++) {
		MinMax candidate = boundingFace[i];
		if ((myPos.x - candidate.maxX) * (myPos.x - candidate.minX) < 0.0f &&
			(myPos.z - candidate.maxZ) * (myPos.z - candidate.minZ) < 0.0f) {
			val.y = candidate.anyYpos + 2.0f;
		}
	}

	//�ٴڿ� ��ġ�Ұ�� y��
	for (int i = 0; i < basement.size(); i++) {
		MinMax candidate = basement[i];
		if ((myPos.x - candidate.maxX) * (myPos.x - candidate.minX) < 0.0f &&
			(myPos.z - candidate.maxZ) * (myPos.z - candidate.minZ) < 0.0f) {
			val.y = candidate.anyYpos + 2.0f;
		}
	}

	return val;
}

void Camera::setBasement(std::vector<MinMax> base)
{
	basement = base;
	
}

bool Camera::getGameOver()
{
	return shouldGameOver;
}





