#include "Transform.h"
#include "Camera.h"
//Transform* Transform::instance = nullptr;



void Transform::calSRT(std::vector <Mainset>& Tree)
{    

    //��� �������� ���� ����ȉ����� ������� �ʴ´�.
    if (!istransingdone) {        
        return;
    }

    //������ �÷��̾ ������
    if (m_cam->isrange()) {
        israge = true;
        firstGetBack = true;//ó�� ���������� ������� ����� ���� ������

        if (istransingdone) {

        if (walking == true) {//�氡���� �÷��̾����� �;��Ҷ�            
            bArrivedAtCheckpoint = false;
            walking = false;
            changeOccurePoint = objectPos;
            XMFLOAT3 checkpoint;
            XMStoreFloat3(&checkpoint, changeOccurePoint);
            footprint.push(checkpoint);
        }
        else {//�̹� ������ ����������
            
        }
            ComeOnMe(Tree);
        }


    }
    else {//���� ���϶�
        israge = false;
        if (istransingdone == true) {

            if (!walking) {//�־������ ���ư����Ҷ�
                getBack(Tree);
            }
            else {//�־��� ���� �Ȱ�������     

             XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));

             //�־����濡 �ٽ� �շ������� ȸ����� ���
             
             if (!bArrivedAtCheckpoint) {
                 bArrivedAtCheckpoint = true;

                 // ����ȭ (Ȥ�� �𸣴ϱ�)
                 XMVECTOR fromDir = XMVector3Normalize(accumulateRotate);//home���� �ٲ�
                 XMVECTOR toDir = XMVector3Normalize(currentDirection);

                 // Y�� 0���� (���� ȸ��)
                 fromDir = XMVectorSetY(fromDir, 0.0f);
                 toDir = XMVectorSetY(toDir, 0.0f);

                 // ȸ���� ���ϱ�
                 XMVECTOR rotationAxis = XMVector3Cross(fromDir, toDir);

                 // �� ���� ���� ����
                 float angle = XMVectorGetX(XMVector3AngleBetweenNormals(fromDir, toDir));

                 // Ȥ�� ���� 0�̸� (���� �����̰ų� �ݴ�) �⺻ Up��
                 if (XMVector3Equal(rotationAxis, XMVectorZero()))
                     rotationAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

                 // ȸ�����
                 rotationMatrix = XMMatrixRotationAxis(rotationAxis, angle);

                 
             }
             

             walking = true;

            Radian = 0.0f;

            PathFinder& pt = PathFinder::GetInstance();

            //XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));6.08

            if (current == destination) {//�б��� ��ġ�Ҷ�
                int num = pt.iscrossing(destination);
                std::list<Point> graph = pt.getListGraph(num);

                int slectList = rand() % (graph.size());

                std::list<Point>::iterator iter = graph.begin();
                for (int i = 0; i < slectList; i++) {
                    iter++;
                }
                //������ �缳��
                destination.x = iter->x;
                destination.z = iter->z;

                previousstate = currentstate;
                //����>>�����϶� ������� ����Ⱥ��Ҷ�...

                if (destination.x > current.x) {//����
                    currentstate = CurrentState::Xplus;
                    modelForward = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
                    currentDirection= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);  // +X ����
                }
                else if (destination.x < current.x) {//��
                    currentstate = CurrentState::Xminus;
                    modelForward = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
                    currentDirection= XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); // -X ����
                }
                else if (destination.z > current.z) {//��
                    currentstate = CurrentState::Zplus;
                    modelForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
                    currentDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);  // +Z ����
                }
                else {//��
                    currentstate = CurrentState::Zminus;
                    modelForward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
                    currentDirection = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); // -Z ����
                }
                accumulateRotate = currentDirection;//6.13 ���� ���� �����ϱ� ���� ����
                //���� �缳��..
                if (previousstate == CurrentState::Zplus) {//���� ���°� ��
                    if (currentstate == CurrentState::Xplus) {//����
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                    else if (currentstate == CurrentState::Zplus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                }
                else if (previousstate == CurrentState::Zminus) {//�������°� ���϶�
                    if (currentstate == CurrentState::Zplus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Zminus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//����
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                }
                else if (previousstate == CurrentState::Xplus) {//�������°� �����϶�
                    if (currentstate == CurrentState::Zplus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xplus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                }
                else {//�������°� ���϶�
                    if (currentstate == CurrentState::Zplus) {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
                        Radian = 90.0f;
                    }
                    else if (currentstate == CurrentState::Xplus) {//����
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
                        Radian = 180.0f;
                    }
                    else if (currentstate == CurrentState::Xminus) {
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(0.0f));
                        Radian = 0.0f;
                    }
                    else {//��
                        rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
                        Radian = -90.0f;
                    }
                }


                // ������� ���⼳��
                

                //previousstate = currentstate;

                //rotate����..

            }
            else {//���������� �������϶�
                if (currentstate == CurrentState::Xplus) {//����
                    TransX = Speed;
                    TransZ = 0.0f;
                    current.x += Speed;
                    if (current.x > destination.x) current.x = destination.x; // ����
                }
                else if (currentstate == CurrentState::Xminus) {//��
                    TransX = -Speed;
                    TransZ = 0.0f;
                    current.x -= Speed;
                    if (current.x < destination.x) current.x = destination.x; // ����
                }
                else if (currentstate == CurrentState::Zminus) {//��
                    TransX = 0.0f;
                    TransZ = -Speed;
                    current.z -= Speed;
                    if (current.z < destination.z) current.z = destination.z; // ����
                }
                else {//��
                    TransX = 0.0f;
                    TransZ = Speed;
                    current.z += Speed;
                    if (current.z > destination.z) current.z = destination.z; // ����
                }
            }

            XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
            // ���� ��ġ�� �������� �̵� (���� �̵����� �ݴ�� ����)
            XMMATRIX invTranslationMatrix = XMMatrixTranslation(-current.x, 0.0f, -current.z);            

            // ���� ��ġ�� �ٽ� �̵�
            XMMATRIX translationMatrix = XMMatrixTranslation(current.x + TransX, 0.0f, current.z + TransZ);

            worldMatrix = scaleMatrix * invTranslationMatrix * rotationMatrix * translationMatrix;

            objectPos = XMVectorSet(current.x + TransX, 0.0f, current.z + TransZ, 1.0f);//new!!
            }//!!

        }
        else {//���ϴ� ���̱� �ѵ� ���ư����Ҷ�..?
            if (walking == false) {

            }else{
            XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
            // ���� ��ġ�� �������� �̵� (���� �̵����� �ݴ�� ����)
            XMMATRIX invTranslationMatrix = XMMatrixTranslation(-current.x, 0.0f, -current.z);

            // ȸ�� ����
            XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(Radian));

            // ���� ��ġ�� �ٽ� �̵�
            XMMATRIX translationMatrix = XMMatrixTranslation(current.x + TransX, 0.0f, current.z + TransZ);

            worldMatrix = scaleMatrix * invTranslationMatrix * rotationMatrix * translationMatrix;
            objectPos = XMVectorSet(current.x + TransX, 0.0f, current.z + TransZ, 1.0f);//new!!
            }//!!
        }
        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);

        for (int i = 0; i < Tree.size(); i++) {
            //BoundingBox oldBox = Tree[i].Box;

            //�̵� �� �ٿ�� �ڽ� ������Ʈ
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // ũ�� ��ȯ

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

//�ʱⰪ ����
void Transform::defineCurrent()
{

    srand(time(NULL));
    previousstate = CurrentState::Xplus;
    currentstate = CurrentState::Xplus;
    
    //���Ƿ� ������ ����
    current.x = 18;
    current.z=-19;
    //�����Ƿ� ������ ����
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
    // �̵� ���� ��� (����ȭ)
    XMVECTOR direction = XMVectorSubtract(playerPos, objectPos);
    direction = XMVector3Normalize(direction);
    direction = XMVectorScale(direction, 0.1f); // ���� ���� ũ�� 0.1��
    direction = XMVectorSetY(direction, 0.0f); // y ������ 0���� ����
    direction = XMVectorSetW(direction, 0.0f); // w ����

    XMVECTOR tempObjectPos;//������ǥ ����
    XMVECTOR testObjectPosX;
    XMVECTOR testObjectPosZ;
    tempObjectPos = objectPos;
    testObjectPosX = objectPos;
    testObjectPosZ = objectPos;

    float speed = 1.0f;
    
    // 1) �ӷ� ����(��Ÿ) ���
    XMVECTOR delta = XMVectorScale(direction, speed);

    // 2) ��Ÿ�� X, Z ���� ����
    float dx = XMVectorGetX(delta);
    float dz = XMVectorGetZ(delta);
 
    //���ο���ǥ
    objectPos = XMVectorAdd(objectPos, XMVectorScale(direction, speed));
    // w �� ����
    objectPos = XMVectorSetW(objectPos, 1.0f);

    XMFLOAT3 newPosition;
    XMStoreFloat3(&newPosition, objectPos);

    collisionDetected = false;

    for (auto& objectBox : otherBox)//��,����� x
    {
        if (CheckCollision(objectBox.get(), newPosition))
        {            
            collisionDetected = true;
            break;
        }
    }

    float dirX = XMVectorGetX(direction);
    float dirZ = XMVectorGetZ(direction);

    //�̵������� ���� �ȸ����� ������ 
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
        objectPos = tempObjectPos;//���������� ����
        XMStoreFloat3(&position, objectPos);//�浹������ Ǫ��        

        //x�࿡���Ͽ�
        XMFLOAT3 nowPosition;
        XMStoreFloat3(&nowPosition, testObjectPosX);
        for (auto& objectBox : otherBox)//��,����� x
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
        direction = XMVectorScale(direction, 0.1f); // ���� ���� ũ�� 0.1��
        direction = XMVectorSetW(direction, 0.0f); // w ����
        direction = XMVectorSetY(direction, 0.0f); // y ������ 0���� ����        

        worldMatrix = XMMatrixTranslationFromVector(direction);
        
        objectPos = modifiedPos;

        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < Tree.size(); i++) {            
            //�̵� �� �ٿ�� �ڽ� ������Ʈ
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // ũ�� ��ȯ

            XMStoreFloat3(&Tree[i].Box.Center, newCenter);
            XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
        }

        setRange(Tree);
    }
    else {//���� �ȸ�������

        // ���� ��Ʈ���� ���� (XMMATRIX�� ���� ���)             
        // 1) �÷��̾� ���� ����(XZ ��� ���� �� ����ȭ)
        XMVECTOR toPlayer = XMVectorSubtract(playerPos, objectPos);
        toPlayer = XMVectorSetY(toPlayer, 0.0f); // ���� ȸ���� ���
        toPlayer = XMVector3Normalize(toPlayer);
        //�÷��̾� ���¹��� �ݴ���� 6.11
        fromawayPlayerDir = -toPlayer;
        

        // 3) ���� ���
        float cosTheta = XMVectorGetX(XMVector3Dot(modelForward, toPlayer));
        cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f)); // Ŭ����

        // 4) ȸ�� ����(��ȣ) ����
        XMVECTOR crossV = XMVector3Cross(modelForward, toPlayer);
        float sign = (XMVectorGetY(crossV) >= 0.0f) ? +1.0f : -1.0f;

        // 5) ���� yaw ȸ���� (����)
        float yaw = sign * acosf(cosTheta);

        // 6) ȸ�� ��� ����
        XMMATRIX R = XMMatrixRotationY(yaw);
        
        //���� ������ ������ �˱����� ��������
        accumulateRotate = XMVector3TransformNormal(accumulateRotate, R);
        
        // 2) ���� modelForward�� ���� �����ϰ�
//    ���� toPlayer�� �����!
        modelForward = toPlayer;
        
        //���⼭
        XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        // ���� ��ġ�� �������� �̵� (���� �̵����� �ݴ�� ����)
        XMMATRIX invTranslationMatrix = XMMatrixTranslation(-XMVectorGetX(tempObjectPos), 0.0f, -XMVectorGetZ(tempObjectPos));       
        
        XMMATRIX rotationMatrix = XMMatrixRotationY(prevYaw);
        
        // ���� ��ġ�� �ٽ� �̵�
        XMMATRIX translationMatrix = XMMatrixTranslation(XMVectorGetX(tempObjectPos) + dx, 0.0f, XMVectorGetZ(tempObjectPos)+dz);

        worldMatrix = scaleMatrix * invTranslationMatrix * R * translationMatrix;        
        
        //tree ���� name->setRange(tree)        

        for (int i = 0; i < Tree.size(); i++) {    

            //�̵� �� �ٿ�� �ڽ� ������Ʈ
            XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
            XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);// ũ�� ��ȯ
            XMStoreFloat3(&Tree[i].Box.Center, newCenter);
            XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
        }
        setRange(Tree);
    }

    
}

//������ ù������ ���������θ� �浹ü �����Ѵ�.
//���� ����Ҷ��� ���ŵ� ù������ �浹ü�θ� ����Ѵ�.
void Transform::setRange(std::vector<Mainset>& Tree)
{
    if (Tree.empty()) return;

    range = Tree[0].Box;

    // ������ ��ҵ�� ����
    for (size_t i = 1; i < Tree.size(); ++i)
    {
        DirectX::BoundingBox::CreateMerged(range, range, Tree[i].Box);
    }
    // ���յ� BoundingBox Ȯ��
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
    //���Ͱ� �ǵ��ư���  changeOccurePoint�� ���鼭 ���ư����ϹǷ�
    //���� 1ȸ ȸ�� ���ش�
    if (firstGetBack) {
        firstGetBack = false;
        getBackOnce();
        return;
   }
    
    if (!isSteped) {//üũ����Ʈ�� �ǵ��ư������ΰ�?
        XMFLOAT3 stepfloat = footprint.top();
        stepvec = XMLoadFloat3(&stepfloat);
        footprint.pop();
        
        isSteped = true;
    }
    
    // �̵� ���� ��� (����ȭ)
    XMVECTOR direction = XMVectorSubtract(stepvec, objectPos);
    direction = XMVector3Normalize(direction);
    direction = XMVectorScale(direction, 0.1f); // ���� ���� ũ�� 0.1��
    direction = XMVectorSetW(direction, 0.0f); // w ����
    direction = XMVectorSetY(direction, 0.0f); // y ������ 0���� ����


    float speed = 1.0f;
    objectPos = XMVectorAdd(objectPos, XMVectorScale(direction, speed));
    // w �� ����
    objectPos = XMVectorSetW(objectPos, 1.0f);

    constexpr float epsilon = 0.1f; // ���ϴ� ���� ���� ����
    XMVECTOR vEpsilon = XMVectorReplicate(epsilon);
    
    if (XMVector3NearEqual(objectPos, stepvec, vEpsilon)) {
        objectPos = stepvec; // ��Ȯ�� ��ġ�� ����        
        isSteped = false;
    }      
    
    worldMatrix = XMMatrixTranslationFromVector(direction);


    if (XMVector3NearEqual(objectPos, changeOccurePoint, vEpsilon)) {        
        walking = true;       
    }
    XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);      

    for (int i = 0; i < Tree.size(); i++) {
       //�̵� �� �ٿ�� �ڽ� ������Ʈ
        XMVECTOR newCenter = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Center), worldMatrix);
        XMVECTOR newExtents = XMVector3Transform(XMLoadFloat3(&Tree[i].Box.Extents), scaleMatrix);  // ũ�� ��ȯ

        XMStoreFloat3(&Tree[i].Box.Center, newCenter);
        XMStoreFloat3(&Tree[i].Box.Extents, newExtents);
    }
    setRange(Tree);
}

//�� �浹ü ����
void Transform::setMapBox(std::vector<std::reference_wrapper<BoundingBox>> box)
{
    otherBox = box;
}

bool Transform::CheckCollision(const BoundingBox& checkbox, const XMFLOAT3& newPosition)
{
    // ���ο� ��ġ�� ī�޶��� �ٿ�� �ڽ��� �̵�
    BoundingBox newCameraBox;
    newCameraBox.Center = newPosition;
    newCameraBox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);//cameraBoundingBox.Extents;

    // ���ο� ��ġ���� �浹 ���� Ȯ��
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
    // ������ ���� ���Ϳ� ��ǥ ���� ����ȭ
    XMVECTOR vFrom = XMVector3Normalize(accumulateRotate);
    XMVECTOR vTo = XMVector3Normalize(changeOccurePoint);

    // ȸ����� ȸ����
    XMVECTOR axis = XMVector3Normalize(XMVector3Cross(vFrom, vTo));
    float angle = acosf(XMVectorGetX(XMVector3Dot(vFrom, vTo)));

    // ȸ�� ���
    XMMATRIX rotMatrix = XMMatrixRotationAxis(axis, angle);
    accumulateRotate = XMVector3TransformNormal(accumulateRotate, rotMatrix);//���� ����
    //______________   
    
    // ������ �� (���ϴ� ������ ����)
    XMFLOAT3 scaleValue = XMFLOAT3(1.0f, 1.0f, 1.0f);
    XMMATRIX scaleMatrix = XMMatrixScaling(scaleValue.x, scaleValue.y, scaleValue.z);

    // ���� objectPos ������ �̵����
    XMMATRIX translationMatrix = XMMatrixTranslationFromVector(objectPos);

    // objectPos�� ����� (���� �̵�)
    XMMATRIX invTranslationMatrix = XMMatrixTranslationFromVector(-objectPos);

    //ȸ���� �ϴ� ���
    worldMatrix = scaleMatrix * invTranslationMatrix * rotMatrix * translationMatrix;
}
