#include "stdafx.h"
#include "SystemClass.h"
#include "InputClass.h"
#include "GraphicsClass.h"

SystemClass::SystemClass() 
{

}

SystemClass::SystemClass(const SystemClass& other) 
{

}

SystemClass::~SystemClass() 
{

}

bool SystemClass::Initialize() 
{
	int screenWidth = 0;
	int screenHegiht = 0;

	IntiailizeWindows(screenWidth, screenHegiht);

	m_Input = new InputClass;
	if (!m_Input) return false;

	m_Input->Initialize();

	m_Graphics = new GraphicClass;
	if (!m_Graphics) return false;

	return m_Graphics->Initialize(screenWidth, screenHegiht, m_hwnd);
}

void SystemClass::Shutdown() 
{
	if (m_Graphics) 
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	//memset과 같은 하지만 무조건 0으로만 초기화
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		//peekMessage GetMessage와 다르게 실시간으로 입력을 받음
		//true면 입력이 있다는 것이고, false면 입력이 없다는 것이다
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			//msg중 Keyboard입력만 받음
			TranslateMessage(&msg);
			//msg를 WndProc으로 전송
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame()) break;
		}
	}
}

bool SystemClass::Frame()
{
	if (m_Input->IsKeyDown(VK_ESCAPE)) return false;
	return m_Graphics->Frame();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hWnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN: {
			m_Input->KeyDown((UINT)wparam);
			return 0;
		}
		case WM_KEYUP: {
			m_Input->KeyUp((UINT)wparam);
			return 0;
		}
		default:
			//나머지 메세지에 대해서 알아서 처리해줌
			return DefWindowProc(hWnd, umsg, wparam, lparam);
	}
}

void SystemClass::IntiailizeWindows(int& screenWidth, int& screenHeight)
{
	ApplicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"Dx11Demo_02";

	//윈도우 창 옵션
	WNDCLASSEX wc;
	//스타일 지정, 현재 옵션은 업데이트때 가로, 세로 업데이트, 하나의 DC를 독점적으로 사용
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//윈도우 이벤트를 받을 함수의 포인터
	wc.lpfnWndProc = WndProc;
	//여분 메모리의 크기
	wc.cbClsExtra = 0;
	//개별 윈도우에서 사용한 여분 메모리의 크기
	wc.cbWndExtra = 0;
	//윈도우 클래스가 사용하는 인스턴스 핸들
	wc.hInstance = m_hinstance;
	//아이콘 모양 지정
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	//커서 모양 지정
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//윈도우 배경 색상 지정
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//프로그램이 사용할 메뉴를 지정
	wc.lpszMenuName = NULL;
	//윈도우 클래시 이름 지정
	wc.lpszClassName = m_applicationName;
	//윈도우 구조체의 크기를 지정
	wc.cbSize = sizeof(WNDCLASSEX);

	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	int posX = 0, posY = 0;
	if (FULL_SCREEN)
	{
		//환경 또는 디스플레이 장치에 대한 정보
		DEVMODE dmScreenSettings;
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));

		//구조체의 크기
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);

		//디스플레이 장치의 가로 길이
		dmScreenSettings.dmPelsWidth = (UINT)screenWidth;
		//디스플레이 장치의 세로 길이
		dmScreenSettings.dmPelsHeight = (UINT)screenHeight;
		//디스플레이 장치의 색상 해상도
		dmScreenSettings.dmBitsPerPel = 32;
		//어떤 프로퍼티를 설정했는지
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//디스플레이 셋팅 변경 대충 해상도를 바꾸어 준다
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		//현재 스크린의 X크기
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		//현재 스크린의 Y크기
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

		//윈도우 만들기
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
								WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
								posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

		//윈도우 띄우기
		ShowWindow(m_hwnd, SW_SHOW);
		//제일 위에 띄우기
		SetForegroundWindow(m_hwnd);
		//현재 활성화되어 있는 윈도우 변경
		SetFocus(m_hwnd);
	}
}

void SystemClass::ShutdownWindows()
{
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	//윈도우 파괴
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//윈도우 클래스 등록 해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY: {
			//WM_QUIT를 날려줌 단순히 메세지 루프를 벗어나기 위해서
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}
		default:
			return ApplicationHandle->MessageHandler(hWnd, umessage, wparam, lparam);	
	}
}


