#pragma once
#include <iostream>  // �����
#include <vector>    // ���� ���
#include <list>      // ���� ����Ʈ ���
#include <map>       // std::map ���


struct Point {
	float x;
	float z;
	// == ������ �����ε� �߰�
	bool operator==(const Point& other) const {
		return x == other.x && z == other.z;
	}
};

class PathFinder
{
public:
	static PathFinder& GetInstance() {
		if (instance == nullptr) { // nullptr ��� ����
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
	std::vector<std::list<Point>> Map; //���Ḯ��Ʈ
	Point current;
};

