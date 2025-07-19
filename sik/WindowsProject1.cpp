// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject1.h"
#include <iostream>
#include "Engine.h"
#include <dinput.h>
#include "SceneManager.h"
#include <vector>
#include <thread>
#include <atomic>
#include <future>  // std::async, std::future 사용을 위한 헤더

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

struct delta {
    int deltaX; // X축 이동량
    int deltaY; // Y축 이동량
};

// ReadMouse와 InitializeMouse 함수 원형 추가
bool InitializeMouse(HINSTANCE hInstance, HWND hwnd);
delta ReadMouse();
void ShutdownMouse();
void InitEngine(Engine* engine);

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND Global_hwnd;
LPDIRECTINPUT8       g_directInput = nullptr;  // DirectInput 객체
LPDIRECTINPUTDEVICE8 g_mouse = nullptr;       // 마우스 장치
DIMOUSESTATE         g_mouseState;            // 마우스 상태 (델타 값 저장)
std::vector<HBITMAP> hBitmaps;
//std::atomic<bool> isInitComplete = false; // 초기화 완료 여부

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
float deltaX = 0.0f;
float deltaY = 0.0f;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    int width;
    int height;
    RECT clientRect;
    if (GetClientRect(Global_hwnd, &clientRect)) {
        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;
        std::cout << "Client area size: " << width << "x" << height << std::endl;
    }
    else {
        std::cerr << "Failed to get client rect." << std::endl;
    }
    POINT centerPoint;
    centerPoint.x = width / 2;
    centerPoint.y = height / 2;
    ClientToScreen(Global_hwnd, &centerPoint);
    //ShowCursor(false);
    InitializeMouse(hInst, Global_hwnd);
    
    Engine sample(width, height, L"", Global_hwnd);
    sample.OnInit();
    
    
    SceneManager& scene = SceneManager::GetInstance();
    //scene.loadScene(Global_hwnd, clientRect);
    
    

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;
    
    
    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) //메세지가 없어도 항상 반환된다
            //메세지가 있다면 true 없다면 false
        {
            if (msg.message == WM_QUIT) {
                break;
            }
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            //메세지가 없는동안 호출            
                delta moustdelta = ReadMouse();
                sample.OnUpdate(moustdelta.deltaX, moustdelta.deltaY);//업데이트
                if (sample.getshouldMouseCenter()) {
                    SetCursorPos(centerPoint.x, centerPoint.y);
                }
        }
    }



    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = nullptr;//MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   Global_hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!Global_hwnd)
   {
      return FALSE;
   }
   RECT clientRect;
   GetClientRect(Global_hwnd, &clientRect);
   SceneManager& scene = SceneManager::GetInstance();
   scene.loadScene(Global_hwnd, clientRect);
   ShowWindow(Global_hwnd, nCmdShow);
   UpdateWindow(Global_hwnd);
   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static POINT lastMousePos = { 0, 0 };
    static bool firstMouse = true;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    { 

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //여기에 hdc를 사용하는 그리기 코드를 추가합니다
        SceneManager& scene = SceneManager::GetInstance();
        scene.DrawScene(scene.getCurrentScene()); // 여기서 그리기
        EndPaint(hWnd, &ps);        
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOUSEMOVE:
    {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);

        if (firstMouse)
        {
            lastMousePos.x = xPos;
            lastMousePos.y = yPos;
            firstMouse = false;
        }

        deltaX = (xPos - lastMousePos.x);
        deltaY = (yPos - lastMousePos.y);

        lastMousePos.x = xPos;
        lastMousePos.y = yPos;     
    
    }
    break;
    //return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool InitializeMouse(HINSTANCE hInstance, HWND hwnd)
{
    // DirectInput 객체 생성
    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_directInput, nullptr)))
        return false;

    // 마우스 장치 생성
    if (FAILED(g_directInput->CreateDevice(GUID_SysMouse, &g_mouse, nullptr)))
        return false;

    // 데이터 형식 설정 (마우스)
    if (FAILED(g_mouse->SetDataFormat(&c_dfDIMouse)))
        return false;

    // 협조 수준 설정 (백그라운드 + 비제한 모드)
    if (FAILED(g_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;

    // 마우스 장치 활성화
    if (FAILED(g_mouse->Acquire()))
        return false;

    return true;
}

delta ReadMouse()
{
    HRESULT result;

    // 마우스 상태 업데이트
    result = g_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&g_mouseState);
    if (FAILED(result))
    {
        // 입력이 일시적으로 끊긴 경우 다시 획득
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
            g_mouse->Acquire();
        else
        {
            assert(nullptr);
        }
            //eturn false;
    }

    delta del;
    // 마우스 이동량(델타 값)
    del.deltaX = g_mouseState.lX;  // X축 이동량
    del.deltaY = g_mouseState.lY;  // Y축 이동량
    //int deltaZ = g_mouseState.lZ;  // 휠 스크롤 이동량

    // 마우스 버튼 상태 확인 (0: 왼쪽 버튼, 1: 오른쪽 버튼, 2: 휠 클릭, 3: 사이드 버튼)
    bool leftButton = (g_mouseState.rgbButtons[0] & 0x80) != 0;
    bool rightButton = (g_mouseState.rgbButtons[1] & 0x80) != 0;
    bool middleButton = (g_mouseState.rgbButtons[2] & 0x80) != 0;


    return del;
}

void ShutdownMouse()
{
    if (g_mouse)
    {
        g_mouse->Unacquire();
        g_mouse->Release();
        g_mouse = nullptr;
    }

    if (g_directInput)
    {
        g_directInput->Release();
        g_directInput = nullptr;
    }
}
