#include "fps.h"
#include <string>
using namespace std;

// static 멤버 변수 초기화
int fps::frameCnt = 0;
float fps::timeElapsed = 0.0f;
LARGE_INTEGER fps::startTime = { 0 };  // 시작 시간 초기화
LARGE_INTEGER fps::frequency = { 0 };  // 주파수 초기화

// 생성자 정의
fps::fps() {
    //myhwnd = hwnd;
    // 카운터의 주파수를 구함
    QueryPerformanceFrequency(&frequency);
    // 시작 시간 기록
    QueryPerformanceCounter(&startTime);

    srand(time(NULL));
}

bool fps::calculateFrameStats() {
    // 프레임 수 증가
    frameCnt++;

    // 현재 시간과 시작 시간의 차이를 계산
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    // 지난 시간 계산
    deltaTime = static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

    // 5초가 지났을 때 FPS를 계산
    if (deltaTime >= 5.0f) {//5초마다 
        float fpsValue = static_cast<float>(frameCnt);  // FPS 계산
        float mspf = 1000.0f / fpsValue;  // Milliseconds per frame 계산

        //이벤트 발생 유무
        int num = rand()&100;
        if (num <= 4) {//5초마다 5프로 확률로 괴물 등장
           // return true;
        }
        // FPS와 mspf 값을 문자열로 변환
        wstring fpsStr = to_wstring(fpsValue);
        wstring mspfStr = to_wstring(mspf);

        // 창 제목 업데이트
        wstring windowText = L"FPS: " + fpsStr + L"    mspf: " + mspfStr;
        SetWindowText(myhwnd, windowText.c_str());

        // FPS와 시간을 초기화하여 다음 1초를 계산
        frameCnt = 0;
        timeElapsed += deltaTime;
        QueryPerformanceCounter(&startTime);  // 시작 시간을 현재 시간으로 업데이트
    }
    return false;
}

float fps::gettimeElapsed()
{
    return timeElapsed;
}


// 초기화 함수 정의 (fps 객체 생성 및 반환)
fps* initializeFPS() {
    fps* ff = new fps(); // fps 객체 동적 생성
    return ff; // 생성된 fps 객체 반환
}