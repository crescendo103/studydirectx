#include "Transform.h"
#include "Camera.h"
//Transform* Transform::instance = nullptr;



void Transform::calSRT(std::vector <Mainset>& Tree)
{    

    //모든 프레임이 아직 적용안됏을때 계산하지 않는다.
    if (!istransingdone) {        
        return;
    }

    //범위내 플레이어가 있을때
    if (m_cam->isrange()) {
        israge = true;
        firstGetBack = true;//처음 범위내에서 벗어났을때 계산을 위한 오프셋

        if (istransingdone) {

        if (walking == true) {//길가던중 플레이어한테 와야할때            
            bArrivedAtCheckpoint = false;
            walking = false;
            changeOccurePoint = objectPos;
            XMFLOAT3 checkpoint;
            XMStoreFloat3(&checkpoint, changeOccurePoint);
            footprint.push(checkpoint);
        }
        else {//이미 나한테 오고있을때
            
        }
            ComeOnMe(Tree);
        }


    }
    else {//범위 밖일때
        israge = false;
        if (istransingdone == true) {

            if (!walking) {//주어진길로 돌아가야할때
                getBack(Tree);
            }
            else {//주어진 길을 걷고있을때     

             XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));

             //주어진길에 다시 합류했을때 회전행렬 계산
             
             if (!bArrivedAtCheckpoint) {
                 bArrivedAtCheckpoint = true;

                 // 정규화 (혹시 모르니까)
                 XMVECTOR fromDir = XMVector3Normalize(accumulateRotate);//home에서 바뀜
                 XMVECTOR toDir = XMVector3Normalize(currentDirection);

                 // Y값 0으로 (수평만 회전)
                 fromDir = XMVectorSetY(fromDir, 0.0f);
                 toDir = XMVectorSetY(toDir, 0.0f);

                 // 회전축 구하기
                 XMVECTOR rotationAxis = XMVector3Cross(fromDir, toDir);

                 // 두 벡터 사이 각도
                 float angle = XMVectorGetX(XMVector3AngleBetweenNormals(fromDir, toDir));

                 // 혹시 축이 0이면 (같은 방향이거나 반대) 기본 Up축
                 if (XMVector3Equal(rotationAxis, XMVectorZero()))
                     rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

                 // 회전행렬
                 rotationMatrix = XMMatrixRotationAxis(rotationAxis, angle);

                 
             }
             

             walking = true;

            Radian = 0.0f;

            PathFinder& pt = PathFinder::GetInstance();

            //XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));6.08

            if (current == destination) {//분기점 위치할때
                int num = pt.iscrossing(destination);
                std::list<Point> graph = pt.getListGraph(num);

                int slectList = rand() % (graph.size());

                std::list<Point>::iterator iter = graph.begin();
                for (int i = 0; i < slectList; i++) {
                    iter++;
                }
                //목적지 재설정
                destination.x = iter->x;
                destination.z = iter->z;

                previousstate = currentstate;
                //직진>>직진일때 수정요망 방향안변할때...

                if (destination.x > current.x) {//오른
                    currentstate = CurrentState::Xplus;
                    modelForward = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
                    currentDirection= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);  // +X 방향
                }
                else if (destination.x < current.x) {//왼
                    currentstate = CurrentState::Xminus;
                    modelForward = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
                    currentDirection= XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); // -X 방향
                }
                else if (destination.z > current.z) {//뒤
                    currentstate = CurrentState::Zplus;
                    modelForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
                    currentDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);  // +Z 방향
                }
                else {//앞
                    currentstate = CurrentState::Zminus;
                    modelForward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
                    currentDirection = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); // -Z 방향
                }
                accumulateRotate = currentDirection;//6.13 현재 방향 추적하기 위한 벡터
                //방향 재설정..
                if (previousstate == CurrentState::Zplus) {//이전 상태가 앞
                    if (currentstate == CurrentState::Xplus) {//오른
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//왼
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                    else if (currentstate == CurrentState::Zplus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//뒤
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                }
                else if (previousstate == CurrentState::Zminus) {//이전상태가 뒤일때
                    if (currentstate == CurrentState::Zplus) {//앞
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//왼
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Zminus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//오른
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                }
                else if (previousstate == CurrentState::Xplus) {//이전상태가 오른일때
                    if (currentstate == CurrentState::Zplus) {//앞
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//왼
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xplus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//뒤
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                }
                else {//이전상태가 왼일때
                    if (currentstate == CurrentState::Zplus) {//앞
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Xplus) {//오른
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//뒤
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                }


                // 여기까지 방향설정
                

                //previousstate = currentstate;

                //rotate설정..

            }
            else {//꼭지점으로 가는중일때
                if (currentstate == CurrentState::Xplus) {//오른
                    TransX = Speed;
                    TransZ = 0.0f;
                    current.x += Speed;
                    if (current.x > destination.x) current.x = destination.x; // 보정
                }
                else if (currentstate == CurrentState::Xminus) {//왼
                    TransX = -Speed;
                    TransZ = 0.0f;
                    current.x -= Speed;
                    if (current.x < destination.x) current.x = destination.x; // 보정
                }
                else if (currentstate == CurrentState::Zminus) {//뒤
                    TransX = 0.0f;
                    TransZ = -Speed;
                    current.z -= Speed;
                    if (current.z < destination.z) current.z = destination.z; // 보정
                }
                else {//앞
                    TransX = 0.0f;
                    TransZ = Speed;
                    current.z += Speed;
                    if (current.z > destination.z) current.z = destination.z; // 보정
                }
            }

            XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
            // 기존 위치를 원점으로 이동 (이전 이동값을 반대로 적용)
            XMMATRIX invTranslationMatrix = XMMatrixTranslation(-current.x, 0.0f, -current.z);            

            // 원래 위치로 다시 이동
            XMMATRIX translationMatrix = XMMatrixTranslation(current.x + TransX, 0.0f, current.z + TransZ);

            worldMatrix = scaleMatrix * invTranslationMatrix * rotationMatrix * translationMatrix;

            objectPos = XMVectorSet(current.x + TransX, 0.0f, current.z + TransZ, 1.0f);//new!!
            }//!!

        }
        else {//변하는 중이긴 한데 돌아가야할때..?
            if (walking == false) {

            }else{
            XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
            // 기존 위치를 원점으로 이동 (이전 이동값을 반대로 적용)
            XMMATRIX invTranslationMatrix = XMMatrixTranslation(-current.x, 0.0f, -current.z);

            // 회전 적용
            XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(Radian));

            // 원래 위치로 다시 이동
            XMMATRIX translationMatrix = XMMatrixTranslation(current.x + TransX, 0.0f, current.z + TransZ);

            worldMatrix = scaleMatrix * invTranslationMatrix * rotationMatrix * translationMatrix;
            objectPos = XMVectorSet(current.x + TransX, 0.0f, current.z + TransZ, 1.0f);//new!!
            }//!!
        }
        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

        for (int i = 0; i < Tree.size(); i++) {
            //BoundingBox oldBox = Tree[i].Box;

            //이동 후 바운딩 박스 업데이트
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // 크기 변환

            XMStoreFloat3(&Tree[i].Box.Center, newCenter);
            XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
        }

        setRange(Tree);
    }
        
    
    //setRange(Tree);
    
}

XMMATRIX Transform::getMATRIX()
{
    return worldMatrix;
    //return XMMatrixTranslationFromVector(m_accumulatedTranslation);
}

void Transform::setBoundingBoxs(std::vector<BoundingBox> box)
{
    Boxs = box;
}

void Transform::TranslateVertices(std::vector<WaveFrontReader<index_t>::Vertex>& vertices, float transY)
{
    XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, transY, 0.0f);

    for (auto& vertex : vertices) {
        XMVECTOR pos = XMLoadFloat3(&vertex.position);
        pos = XMVector3Transform(pos, translationMatrix);
        XMStoreFloat3(&vertex.position, pos);
    }
}

int Transform::rollthedice()
{
    return rand()%4;
}

void Transform::setName(std::string name)
{
    m_Name = name;
}

std::string Transform::getName()
{
    return m_Name;
}

void Transform::finishedRotate(bool isturnning)
{
    rotatedone = isturnning;
}

//초기값 설정
void Transform::defineCurrent()
{

    srand(time(NULL));
    previousstate = CurrentState::Xplus;
    currentstate = CurrentState::Xplus;
    
    //임의로 시작점 설정
    current.x = 18;
    current.z=-19;
    //임의의로 도착점 설정
    destination.x = 26;
    destination.z = -19;
}

void Transform::finishedTrans(bool isfinished)
{
    istransingdone = isfinished;
}

void Transform::setCampointer(Camera* cam)
{
    m_cam = cam;
}

void Transform::ComeOnMe(std::vector <Mainset>& Tree)
{
    
    XMVECTOR playerPos = m_cam->getMyPosition();    
    // 이동 방향 계산 (정규화)
    XMVECTOR direction = XMVectorSubtract(playerPos, objectPos);
    direction = XMVector3Normalize(direction);
    direction = XMVectorScale(direction, 0.1f); // 방향 벡터 크기 0.1배
    direction = XMVectorSetY(direction, 0.0f); // y 성분을 0으로 설정
    direction = XMVectorSetW(direction, 0.0f); // w 보정

    XMVECTOR tempObjectPos;//이전좌표 저장
    XMVECTOR testObjectPosX;
    XMVECTOR testObjectPosZ;
    tempObjectPos = objectPos;
    testObjectPosX = objectPos;
    testObjectPosZ = objectPos;

    float speed = 1.0f;
    
    // 1) 속력 벡터(델타) 계산
    XMVECTOR delta = XMVectorScale(direction, speed);

    // 2) 델타의 X, Z 성분 추출
    float dx = XMVectorGetX(delta);
    float dz = XMVectorGetZ(delta);
 
    //새로운좌표
    objectPos = XMVectorAdd(objectPos, XMVectorScale(direction, speed));
    // w 값 보정
    objectPos = XMVectorSetW(objectPos, 1.0f);

    XMFLOAT3 newPosition;
    XMStoreFloat3(&newPosition, objectPos);

    collisionDetected = false;

    for (auto& objectBox : otherBox)//길,벽통과 x
    {
        if (CheckCollision(objectBox.get(), newPosition))
        {            
            collisionDetected = true;
            break;
        }
    }

    float dirX = XMVectorGetX(direction);
    float dirZ = XMVectorGetZ(direction);

    //이동했을때 벽에 안막히는 곳으로 
    if (dirX >= 0) {
        testObjectPosX = XMVectorSetX(testObjectPosX, XMVectorGetX(testObjectPosX) + 0.10f);
        isXplus = true;
    }
    else {
        testObjectPosX = XMVectorSetX(testObjectPosX, XMVectorGetX(testObjectPosX) + (-0.10f));
        isXplus = false;
    }

    if (dirZ >= 0) {
        testObjectPosZ = XMVectorSetZ(testObjectPosZ, XMVectorGetZ(testObjectPosZ) + 0.10f);
        isZplus = true;
    }
    else {
        testObjectPosZ = XMVectorSetZ(testObjectPosZ, XMVectorGetZ(testObjectPosZ) + (-0.10f));        
        isZplus = false;
    }


    XMFLOAT3 position;

    XMFLOAT3 nowPositionX;
    XMStoreFloat3(&nowPositionX, testObjectPosX);

    XMFLOAT3 nowPositionZ;
    XMStoreFloat3(&nowPositionZ, testObjectPosZ);

    if (collisionDetected) {
        objectPos = tempObjectPos;//이전값으로 복귀
        XMStoreFloat3(&position, objectPos);//충돌이전값 푸쉬        

        //x축에대하여
        XMFLOAT3 nowPosition;
        XMStoreFloat3(&nowPosition, testObjectPosX);
        for (auto& objectBox : otherBox)//길,벽통과 x
        {
            if (CheckCollision(objectBox.get(), nowPosition))
            {                
                collisionDetectedX = true;
                break;
            }
        }

        XMVECTOR modifiedPos;

        if (collisionDetectedX) {
            modifiedPos = testObjectPosZ;
        }
        else {
            modifiedPos = testObjectPosX;
        }        
        
        direction = XMVectorSubtract(modifiedPos, objectPos);
        direction = XMVector3Normalize(direction);
        direction = XMVectorScale(direction, 0.1f); // 방향 벡터 크기 0.1배
        direction = XMVectorSetW(direction, 0.0f); // w 보정
        direction = XMVectorSetY(direction, 0.0f); // y 성분을 0으로 설정        

        worldMatrix = XMMatrixTranslationFromVector(direction);
        
        objectPos = modifiedPos;

        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < Tree.size(); i++) {            
            //이동 후 바운딩 박스 업데이트
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // 크기 변환

            XMStoreFloat3(&Tree[i].Box.Center, newCenter);
            XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
        }

        setRange(Tree);
    }
    else {//벽에 안막혔을때

        // 월드 매트릭스 갱신 (XMMATRIX로 직접 계산)             
        // 1) 플레이어 방향 벡터(XZ 평면 투영 후 정규화)
        XMVECTOR toPlayer = XMVectorSubtract(playerPos, objectPos);
        toPlayer = XMVectorSetY(toPlayer, 0.0f); // 수평 회전만 고려
        toPlayer = XMVector3Normalize(toPlayer);
        //플레이어 보는방향 반대방향 6.11
        fromawayPlayerDir = -toPlayer;
        

        // 3) 각도 계산
        float cosTheta = XMVectorGetX(XMVector3Dot(modelForward, toPlayer));
        cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f)); // 클램핑

        // 4) 회전 방향(부호) 결정
        XMVECTOR crossV = XMVector3Cross(modelForward, toPlayer);
        float sign = (XMVectorGetY(crossV) >= 0.0f) ? +1.0f : -1.0f;

        // 5) 최종 yaw 회전값 (라디안)
        float yaw = sign * acosf(cosTheta);

        // 6) 회전 행렬 생성
        XMMATRIX R = XMMatrixRotationY(yaw);
        
        //현재 몬스터의 방향을 알기위한 누적벡터
        accumulateRotate = XMVector3TransformNormal(accumulateRotate, R);
        
        // 2) 기존 modelForward는 이제 무시하고
//    곧장 toPlayer로 덮어쓴다!
        modelForward = toPlayer;
        
        //여기서
        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        // 기존 위치를 원점으로 이동 (이전 이동값을 반대로 적용)
        XMMATRIX invTranslationMatrix = XMMatrixTranslation(-XMVectorGetX(tempObjectPos), 0.0f, -XMVectorGetZ(tempObjectPos));       
        
        XMMATRIX rotationMatrix = XMMatrixRotationY(prevYaw);
        
        // 원래 위치로 다시 이동
        XMMATRIX translationMatrix = XMMatrixTranslation(XMVectorGetX(tempObjectPos) + dx, 0.0f, XMVectorGetZ(tempObjectPos)+dz);

        worldMatrix = scaleMatrix * invTranslationMatrix * R * translationMatrix;        
        
        //tree 갱신 name->setRange(tree)        

        for (int i = 0; i < Tree.size(); i++) {    

            //이동 후 바운딩 박스 업데이트
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);// 크기 변환
            XMStoreFloat3(&Tree[i].Box.Center, newCenter);
            XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
        }
        setRange(Tree);
    }

    
}

//몬스터의 첫프레임 정점정보로만 충돌체 설정한다.
//이후 계산할때도 갱신된 첫프레임 충돌체로만 계산한다.
void Transform::setRange(std::vector<Mainset>& Tree)
{
    if (Tree.empty()) return;

    range = Tree[0].Box;

    // 나머지 요소들과 병합
    for (size_t i = 1; i < Tree.size(); ++i)
    {
        DirectX::BoundingBox::CreateMerged(range, range, Tree[i].Box);
    }
    // 병합된 BoundingBox 확장
    range.Extents.x += 6.0f;
    range.Extents.y += 6.0f;
    range.Extents.z += 6.0f;
}

BoundingBox Transform::getRange()
{
    return range;
}

void Transform::setComingstate(bool state)
{
    comingstate = state;
}

Camera* Transform::getcampointer()
{
    return m_cam;
}

void Transform::getBack(std::vector <Mainset>& Tree)
{
    //몬스터가 되돌아갈때  changeOccurePoint를 보면서 돌아가야하므로
    //최초 1회 회전 해준다
    if (firstGetBack) {
        firstGetBack = false;
        getBackOnce();
        return;
   }
    
    if (!isSteped) {//체크포인트로 되돌아가는중인가?
        XMFLOAT3 stepfloat = footprint.top();
        stepvec = XMLoadFloat3(&stepfloat);
        footprint.pop();
        
        isSteped = true;
    }
    
    // 이동 방향 계산 (정규화)
    XMVECTOR direction = XMVectorSubtract(stepvec, objectPos);
    direction = XMVector3Normalize(direction);
    direction = XMVectorScale(direction, 0.1f); // 방향 벡터 크기 0.1배
    direction = XMVectorSetW(direction, 0.0f); // w 보정
    direction = XMVectorSetY(direction, 0.0f); // y 성분을 0으로 설정


    float speed = 1.0f;
    objectPos = XMVectorAdd(objectPos, XMVectorScale(direction, speed));
    // w 값 보정
    objectPos = XMVectorSetW(objectPos, 1.0f);

    constexpr float epsilon = 0.1f; // 원하는 오차 범위 설정
    XMVECTOR vEpsilon = XMVectorReplicate(epsilon);
    
    if (XMVector3NearEqual(objectPos, stepvec, vEpsilon)) {
        objectPos = stepvec; // 정확한 위치로 보정        
        isSteped = false;
    }      
    
    worldMatrix = XMMatrixTranslationFromVector(direction);


    if (XMVector3NearEqual(objectPos, changeOccurePoint, vEpsilon)) {        
        walking = true;       
    }
    XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);      

    for (int i = 0; i < Tree.size(); i++) {
       //이동 후 바운딩 박스 업데이트
        XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
        XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // 크기 변환

        XMStoreFloat3(&Tree[i].Box.Center, newCenter);
        XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
    }
    setRange(Tree);
}

//벽 충돌체 저장
void Transform::setMapBox(std::vector<std::reference_wrapper<BoundingBox>> box)
{
    otherBox = box;
}

bool Transform::CheckCollision(const BoundingBox& checkbox, const XMFLOAT3& newPosition)
{
    // 새로운 위치로 카메라의 바운딩 박스를 이동
    BoundingBox newCameraBox;
    newCameraBox.Center = newPosition;
    newCameraBox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);//cameraBoundingBox.Extents;

    // 새로운 위치에서 충돌 여부 확인
    return newCameraBox.Intersects(checkbox);
}

XMMATRIX Transform::getRotateMatrix()
{
    return rotationMatrix;
}

bool Transform::getIsRange()
{
    return israge;
}

XMMATRIX Transform::getbase()
{
    return backTobase;
}

void Transform::getBackOnce()
{    
    //613
    // 누적된 방향 벡터와 목표 방향 정규화
    XMVECTOR vFrom = XMVector3Normalize(accumulateRotate);
    XMVECTOR vTo = XMVector3Normalize(changeOccurePoint);

    // 회전축과 회전각
    XMVECTOR axis = XMVector3Normalize(XMVector3Cross(vFrom, vTo));
    float angle = acosf(XMVectorGetX(XMVector3Dot(vFrom, vTo)));

    // 회전 행렬
    XMMATRIX rotMatrix = XMMatrixRotationAxis(axis, angle);
    accumulateRotate = XMVector3TransformNormal(accumulateRotate, rotMatrix);//누적 갱신
    //______________   
    
    // 스케일 값 (원하는 값으로 설정)
    XMFLOAT3 scaleValue = XMFLOAT3(1.0f, 1.0f, 1.0f);
    XMMATRIX scaleMatrix = XMMatrixScaling(scaleValue.x, scaleValue.y, scaleValue.z);

    // 현재 objectPos 값으로 이동행렬
    XMMATRIX translationMatrix = XMMatrixTranslationFromVector(objectPos);

    // objectPos의 역행렬 (음수 이동)
    XMMATRIX invTranslationMatrix = XMMatrixTranslationFromVector(-objectPos);

    //회전만 하는 행렬
    worldMatrix = scaleMatrix * invTranslationMatrix * rotMatrix * translationMatrix;
}
