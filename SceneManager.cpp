#include "SceneManager.h"
#include "DXSampleHelper.h"


SceneManager* SceneManager::instance = nullptr;

void SceneManager::loadScene(HWND hwnd, RECT rect)
{
    m_hwnd = hwnd;
    clientRect = rect;
    // 비트맵 로드 (이미지 파일을 사용하거나 리소스를 로드할 수 있습니다)
    HBITMAP hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // 인스턴스 핸들(NULL이면 파일 경로)
        L"Scene\\loading.bmp",              // BMP 파일 경로
        IMAGE_BITMAP,                // 비트맵 형식
        0, 0,                        // 원본 크기 사용
        LR_LOADFROMFILE              // 파일에서 로드
    );

    if (!hBitmap) {
        MessageBox(nullptr, L"이미지를 불러오지 못했습니다!", L"오류", MB_OK);
    }

    hBitmaps.push_back(hBitmap);

     hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // 인스턴스 핸들(NULL이면 파일 경로)
        L"Scene\\escape_1.bmp",              // BMP 파일 경로
        IMAGE_BITMAP,                // 비트맵 형식
        0, 0,                        // 원본 크기 사용
        LR_LOADFROMFILE              // 파일에서 로드
    );

    hBitmaps.push_back(hBitmap);

     hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // 인스턴스 핸들(NULL이면 파일 경로)
        L"Scene\\gameover_1.bmp",              // BMP 파일 경로
        IMAGE_BITMAP,                // 비트맵 형식
        0, 0,                        // 원본 크기 사용
        LR_LOADFROMFILE              // 파일에서 로드
    );

    hBitmaps.push_back(hBitmap);
}

void SceneManager::DrawScene(int currentScene)
{
        HDC hdc = GetDC(m_hwnd);

        // 비트맵을 메모리 DC에 선택
        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, hBitmaps[static_cast<int>(currentScene)]);

        // 비트맵 크기 가져오기
        BITMAP bitmap;
        GetObject(hBitmaps[static_cast<int>(currentScene)], sizeof(BITMAP), &bitmap);

        int winWidth = clientRect.right - clientRect.left;
        int winHeight = clientRect.bottom - clientRect.top;

        // 화면에 출력
        StretchBlt(hdc, 0, 0, winWidth, winHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

        // 메모리 DC 해제
        DeleteDC(memDC);    

        ReleaseDC(m_hwnd, hdc); // DC 해제
    
}

void SceneManager::setState(int scenenumber)
{
    currentScene = scenenumber;
}

int SceneManager::getCurrentScene()
{
    return currentScene;
}

void SceneManager::setswapchain(ComPtr<IDXGISwapChain3> swapChain)
{
    m_swapChain = swapChain;
}

std::vector<HBITMAP> SceneManager::getmaps()
{
    return hBitmaps;
}

RECT SceneManager::getrect()
{
    return clientRect;
}
