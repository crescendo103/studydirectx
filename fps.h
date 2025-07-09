#pragma once
#include <windows.h>
#include <string>
#include<iostream>
#include<cstdlib>
#include<ctime>

class fps {
public:
    static int frameCnt;  // ������ ��
    static float timeElapsed;  // ���� �ð�
    HWND myhwnd;  // ������ �ڵ�
    float deltaTime;

public:
     fps();  // ������
     bool calculateFrameStats();  // FPS ��� �Լ�
     static float gettimeElapsed();
    

private:
    static LARGE_INTEGER startTime;  // ���� �ð�
    static LARGE_INTEGER frequency;  // ī������ ���ļ�
    
};

// �ʱ�ȭ �Լ� ���� (fps ��ü ��ȯ)
fps* initializeFPS();