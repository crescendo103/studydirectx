#pragma once
#include <windows.h>
#include <string>
#include<iostream>
#include<cstdlib>
#include<ctime>

class fps {
public:
    static int frameCnt;  // 프레임 수
    static float timeElapsed;  // 지난 시간
    HWND myhwnd;  // 윈도우 핸들
    float deltaTime;

public:
     fps();  // 생성자
     bool calculateFrameStats();  // FPS 계산 함수
     static float gettimeElapsed();
    

private:
    static LARGE_INTEGER startTime;  // 시작 시간
    static LARGE_INTEGER frequency;  // 카운터의 주파수
    
};

// 초기화 함수 선언 (fps 객체 반환)
fps* initializeFPS();