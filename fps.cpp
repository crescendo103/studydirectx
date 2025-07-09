#include "fps.h"
#include <string>
using namespace std;

// static ��� ���� �ʱ�ȭ
int fps::frameCnt = 0;
float fps::timeElapsed = 0.0f;
LARGE_INTEGER fps::startTime = { 0 };  // ���� �ð� �ʱ�ȭ
LARGE_INTEGER fps::frequency = { 0 };  // ���ļ� �ʱ�ȭ

// ������ ����
fps::fps() {
    //myhwnd = hwnd;
    // ī������ ���ļ��� ����
    QueryPerformanceFrequency(&frequency);
    // ���� �ð� ���
    QueryPerformanceCounter(&startTime);

    srand(time(NULL));
}

bool fps::calculateFrameStats() {
    // ������ �� ����
    frameCnt++;

    // ���� �ð��� ���� �ð��� ���̸� ���
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    // ���� �ð� ���
    deltaTime = static_cast<float>(currentTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

    // 5�ʰ� ������ �� FPS�� ���
    if (deltaTime >= 5.0f) {//5�ʸ��� 
        float fpsValue = static_cast<float>(frameCnt);  // FPS ���
        float mspf = 1000.0f / fpsValue;  // Milliseconds per frame ���

        //�̺�Ʈ �߻� ����
        int num = rand()&100;
        if (num <= 4) {//5�ʸ��� 5���� Ȯ���� ���� ����
           // return true;
        }
        // FPS�� mspf ���� ���ڿ��� ��ȯ
        wstring fpsStr = to_wstring(fpsValue);
        wstring mspfStr = to_wstring(mspf);

        // â ���� ������Ʈ
        wstring windowText = L"FPS: " + fpsStr + L"    mspf: " + mspfStr;
        SetWindowText(myhwnd, windowText.c_str());

        // FPS�� �ð��� �ʱ�ȭ�Ͽ� ���� 1�ʸ� ���
        frameCnt = 0;
        timeElapsed += deltaTime;
        QueryPerformanceCounter(&startTime);  // ���� �ð��� ���� �ð����� ������Ʈ
    }
    return false;
}

float fps::gettimeElapsed()
{
    return timeElapsed;
}


// �ʱ�ȭ �Լ� ���� (fps ��ü ���� �� ��ȯ)
fps* initializeFPS() {
    fps* ff = new fps(); // fps ��ü ���� ����
    return ff; // ������ fps ��ü ��ȯ
}