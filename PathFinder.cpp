#include "PathFinder.h"


PathFinder* PathFinder::instance = nullptr;

void PathFinder::InitMap()
{
    //10�� ������ ����
    //�迭���� x��ǥ, z��ǥ�� �������ִ�
    //�� �迭�� ù x,z��ǥ�� ������ ��ǥ���� ����
    //������ ��ǥ�� �� ��ǥ���� ���������� ����� ������ ������ ��ǥ�� ��Ÿ����
    // ex float a[10] = { 26, 19, 26, 25, 26, 7, 18, 19 };
    //(18,19)
    // ^
    //  |
    //(26,19)------------->(26,25)
    //  |
    // V
    //(26,7)
    {//���̵�����
        float a[10] = { 18, 19, 26, 19 };
        repeatList(a, 2);
    }

    {
        float a[10] = { 18, 19, 26, 19 };
        repeatList(a, 2);
    }
    {
        float a[10] = { 26, 19, 26, 25, 26, 7, 18, 19 };
        repeatList(a, 4);
    }
    {
        float a[10] = { 26, 25, 5, 25, 26, 32, 26, 19 };
        repeatList(a, 4);
    }
    {
        float a[10] = { 26, 32, 26, 25, 12, 32 };
        repeatList(a, 3);
    }
    {
        float a[10] = { 5, 25, 26, 25, 5, 40 };
        repeatList(a, 3);
    }
    {
        float a[10] = { 12, 32, 26, 32, 12, 40 };
        repeatList(a, 3);
    }
    {
        float a[10] = { 12, 40, 12, 32, 5, 40 };
        repeatList(a, 3);
    }
    {
        float a[10] = { 5, 40, 5, 25, 12, 40,-1,40 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -1, 40, -1, 27, 5, 40, -27, 40 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -1, 27, -1, 40 };
        repeatList(a, 2);
    }
    {
        float a[10] = { 26, 7, 26, 19, 5, 7, 26, 0 };
        repeatList(a, 4);
    }
    {
        float a[10] = { 26, 0, 26, 7, 5, 0 };
        repeatList(a, 3);
    }
    {
        float a[10] = { 5, 0, -14, 0, 5, 7, 26, 0 };
        repeatList(a, 4);
    }
    {
        float a[10] = { 5, 7, 5, 0, -8, 7, 26, 7 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -14, 0, -27, 0, -14, 7, 5, 0 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -27, 0, -14, 0, -27, 7 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -14, 7, -14, 0, -27, 7 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -27, 7, -14, 7, -27, 0 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -27, 14, -27, 7, -14, 14, -27, 27 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -8, 7, 5, 7, -8, 19 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -8, 19, -8, 7, -14, 19 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -14, 19, -8, 19, -14, 14 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -14, 14, -27, 14, -14, 19 };
        repeatList(a, 3);
    }
    {
        float a[10] = { -27, 27, -7, 14, -27, 14, -27, 40 };
        repeatList(a, 4);
    }
    {
        float a[10] = { -7, 27, -27, 27 };
        repeatList(a, 2);
    }
    {
        float a[10] = { -27, 40, -27, 27, -1, 40 };
        repeatList(a, 3);
    }

    current.x = 18;
    current.z = -19;
    Map;
}

//�迭�� ����Ʈ�� ����� ���Ϳ� �����ϴ� �Լ�
void PathFinder::repeatList(float a[10], int size)
{
    //�迭�� ����Ʈ�� ����� ����
	std::list<Point> temp;
	for (int i = 0; i < size; i++) {
		Point point;
		point.x = a[2*i]; 
		point.z = -a[2*i + 1];//ǥ��� +z�� ������ �÷��̾�������δ� -z���̴�.
		temp.push_back(point);
	}
	
	Map.push_back(temp);//���� ����
}

Point PathFinder::getCurrent()
{
    return current;
}

void PathFinder::setCurrent(Point point)
{
    current = point;
}

//���Ͱ� ��ǥ �������� �����ؼ� �� ��ǥ �������� ã���� �����ϴ� �Լ�
int PathFinder::iscrossing(Point point)
{
    for (int i = 1; i < Map.size(); i++) {//���͸� ���鼭 ù��° ��ǥ�� point�� ��ġ�ϴ� list �ε��� ��ȣ ����
        if (Map[i].front() == point) {//���� �ش縮��Ʈ���� ��ǥ ������ ���� ����
            return i;
        }
    }
    return 0;//�������
}

std::list<Point> PathFinder::getListGraph(int num)
{
    return Map[num];
}
