#include "PathFinder.h"


PathFinder* PathFinder::instance = nullptr;

void PathFinder::InitMap()
{
    //10은 임의의 숫자
    //배열에는 x좌표, z좌표가 번갈아있다
    //각 배열의 첫 x,z좌표와 다음의 좌표들은 각각
    //교차점 좌표와 그 좌표에서 설정가능한 경로의 목적지 교차점 좌표를 나타낸다
    // ex float a[10] = { 26, 19, 26, 25, 26, 7, 18, 19 };
    //(18,19)
    // ^
    //  |
    //(26,19)------------->(26,25)
    //  |
    // V
    //(26,7)
    {//더미데이터
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

//배열을 리스트로 만들어 벡터에 저장하는 함수
void PathFinder::repeatList(float a[10], int size)
{
    //배열을 리스트로 만드는 과정
	std::list<Point> temp;
	for (int i = 0; i < size; i++) {
		Point point;
		point.x = a[2*i]; 
		point.z = -a[2*i + 1];//표기는 +z로 했지만 플레이어기준으로는 -z축이다.
		temp.push_back(point);
	}
	
	Map.push_back(temp);//벡터 저장
}

Point PathFinder::getCurrent()
{
    return current;
}

void PathFinder::setCurrent(Point point)
{
    current = point;
}

//몬스터가 목표 교차점에 도달해서 새 목표 교차점을 찾을때 실행하는 함수
int PathFinder::iscrossing(Point point)
{
    for (int i = 1; i < Map.size(); i++) {//벡터를 돌면서 첫번째 좌표가 point와 일치하는 list 인덱스 번호 추출
        if (Map[i].front() == point) {//이후 해당리스트에서 목표 교차점 새로 지정
            return i;
        }
    }
    return 0;//추출실패
}

std::list<Point> PathFinder::getListGraph(int num)
{
    return Map[num];
}
