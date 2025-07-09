#pragma once
#include <iostream>  // 입출력
#include <vector>    // 벡터 사용
#include <list>      // 연결 리스트 사용
#include <map>       // std::map 사용


struct Point {
	float x;
	float z;
	// == 연산자 오버로딩 추가
	bool operator==(const Point& other) const {
		return x == other.x && z == other.z;
	}
};

class PathFinder
{
public:
	static PathFinder& GetInstance() {
		if (instance == nullptr) { // nullptr 사용 권장
			instance = new PathFinder();
		}
		return *instance;
	}

	

	void InitMap();
	void repeatList(float a[10], int size);
	Point getCurrent();
	void setCurrent(Point point);
	int iscrossing(Point point);
	std::list<Point> getListGraph(int num);

private:
	static PathFinder* instance;
	PathFinder() {}
	~PathFinder() {}
	std::vector<std::list<Point>> Map; //연결리스트
	Point current;
};

