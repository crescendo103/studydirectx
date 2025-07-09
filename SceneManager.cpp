#include "SceneManager.h"
#include "DXSampleHelper.h"


SceneManager* SceneManager::instance = nullptr;

void SceneManager::loadScene(HWND hwnd, RECT rect)
{
    m_hwnd = hwnd;
    clientRect = rect;
    // ��Ʈ�� �ε� (�̹��� ������ ����ϰų� ���ҽ��� �ε��� �� �ֽ��ϴ�)
    HBITMAP hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // �ν��Ͻ� �ڵ�(NULL�̸� ���� ���)
        L"Scene\\loading.bmp",              // BMP ���� ���
        IMAGE_BITMAP,                // ��Ʈ�� ����
        0, 0,                        // ���� ũ�� ���
        LR_LOADFROMFILE              // ���Ͽ��� �ε�
    );

    if (!hBitmap) {
        MessageBox(nullptr, L"�̹����� �ҷ����� ���߽��ϴ�!", L"����", MB_OK);
    }

    hBitmaps.push_back(hBitmap);

     hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // �ν��Ͻ� �ڵ�(NULL�̸� ���� ���)
        L"Scene\\escape_1.bmp",              // BMP ���� ���
        IMAGE_BITMAP,                // ��Ʈ�� ����
        0, 0,                        // ���� ũ�� ���
        LR_LOADFROMFILE              // ���Ͽ��� �ε�
    );

    hBitmaps.push_back(hBitmap);

     hBitmap = (HBITMAP)LoadImage(
        nullptr,                     // �ν��Ͻ� �ڵ�(NULL�̸� ���� ���)
        L"Scene\\gameover_1.bmp",              // BMP ���� ���
        IMAGE_BITMAP,                // ��Ʈ�� ����
        0, 0,                        // ���� ũ�� ���
        LR_LOADFROMFILE              // ���Ͽ��� �ε�
    );

    hBitmaps.push_back(hBitmap);
}

void SceneManager::DrawScene(int currentScene)
{
        HDC hdc = GetDC(m_hwnd);

        // ��Ʈ���� �޸� DC�� ����
        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, hBitmaps[static_cast<int>(currentScene)]);

        // ��Ʈ�� ũ�� ��������
        BITMAP bitmap;
        GetObject(hBitmaps[static_cast<int>(currentScene)], sizeof(BITMAP), &bitmap);

        int winWidth = clientRect.right - clientRect.left;
        int winHeight = clientRect.bottom - clientRect.top;

        // ȭ�鿡 ���
        StretchBlt(hdc, 0, 0, winWidth, winHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

        // �޸� DC ����
        DeleteDC(memDC);    

        ReleaseDC(m_hwnd, hdc); // DC ����
    
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
