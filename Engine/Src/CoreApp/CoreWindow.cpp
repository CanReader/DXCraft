#include "CoreWindow.h"
#include <windowsx.h>
#include "../../../Client/resource.h"

CoreWindow::CoreWindow(HINSTANCE hInstance) : graph{ std::make_unique<Graphics>() } { this->hInstance = hInstance; }

CoreWindow::~CoreWindow()
{
	DestroyWindow();
}

void CoreWindow::SetFullScreen(bool fullscreen)
{
	DEVMODE dmScreenSettings;
	int posX, posY;

	if (fullscreen)
	{
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = this->Siz.Width;
		dmScreenSettings.dmPelsHeight = this->Siz.Height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		this->Loc.X = 0;
		this->Loc.Y = 0;
	}
	else
	{
		this->Loc.X = (GetSystemMetrics(SM_CXSCREEN) - this->Siz.Width) / 2;
		this->Loc.Y = (GetSystemMetrics(SM_CYSCREEN) - this->Siz.Height) / 2;
	}
		graph->SetFullscreen(fullscreen);
		Windowed = !fullscreen;
}

HWND CoreWindow::GetHWND()
{ 
	return hWnd;
}

HINSTANCE CoreWindow::GetHinstance()
{
	return hInstance;
}

WNDCLASSEX CoreWindow::CreateWindowClass(LPCSTR ClassName,LPCSTR MenuName)
{
	WNDCLASSEX wnd;
	ZeroMemory(&wnd,sizeof(WNDCLASSEX));
	wnd.cbClsExtra = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbWndExtra = NULL;
	wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wnd.hCursor = LoadCursor(hInstance,IDC_ARROW);
	wnd.hIcon = static_cast<HICON>(LoadImage(hInstance,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,0));
	wnd.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = ClassName;
	wnd.style = CS_VREDRAW | CS_HREDRAW;

	this->ClassName = ClassName;

	RegisterClassEx(&wnd);

	return wnd;
}

bool CoreWindow::InitializeWindow(LPCSTR WindowName,int Width,int Height)
{
	Messanger::Log::Initialize();

	CoreWindow::WindowHandle = this;
	this->WindowName = WindowName;

	CreateWindowClass(WindowName,NULL);

	RECT rect = {0,0,Width,Height};
	AdjustWindowRectEx(&rect,WS_OVERLAPPEDWINDOW,NULL,NULL);

	hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		this->ClassName,
		this->WindowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)return false;

	if (!graph->Initialize(hWnd,hInstance, Width, Height,!this->Windowed))
		return false;

	DX_INFO("Graphic class initialized");

	EventHandler = EventManager::getManager();

	SetFullScreen(false);

	this->DisplayWindow(true);

	return true;
}

void CoreWindow::DisplayWindow(bool Show)
{
	if(Show)ShowWindow(hWnd,SW_SHOW);
	else
	{
#pragma warning(disable:4996)

		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		char NowText[9] = "        ";

	ShowWindow(hWnd, SW_HIDE);
	}

	UpdateWindow(hWnd);

	this->Visible = Show;
}

void CoreWindow::ProcessMessages()
{
	ZeroMemory(&Messanger,sizeof(MSG));

	HCURSOR arrow = LoadCursor(NULL, IDC_ARROW);

	if (arrow != NULL)
		SetCursor(arrow);

	while(Messanger.message != WM_QUIT)
	{
		if (PeekMessage(&Messanger, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Messanger);
			DispatchMessage(&Messanger);
			
			if (Messanger.message == WM_QUIT)
				break;
		}

	}
		ReturnValue = Messanger.wParam;
}

void CoreWindow::DestroyWindow()
{
	hWnd = NULL;
	hInstance = NULL;
	Visible = false;

	graph->Shutdown();

	PostQuitMessage(0);
}

bool CoreWindow::isinShow()
{
	return Visible;
}

void CoreWindow::SetWindowTitle(const LPSTR& text)
{
	SetWindowText(hWnd,text);
}

void CoreWindow::SetLocation(int X, int Y)
{
	this->Loc.X = X;
	this->Loc.Y = Y;

	MoveWindow(hWnd, X, Y, Siz.Width, Siz.Height, false);
}

RECT CoreWindow::GetLocation()
{
	RECT rect;
	rect.left = this->Loc.X;
	rect.top = this->Loc.Y;

	return rect;
}

LRESULT CALLBACK CoreWindow::WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_PAINT:
	{
		CoreWindow::WindowHandle->graph->Render();
		
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onWindowPaint)
			if (func != nullptr)
				func();
	}break;

	case WM_ACTIVATE: {
		bool LostFocus = LOWORD(wParam) == WA_INACTIVE;
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onWindowFocusChanged)
			if (func != nullptr)
				func(LostFocus);
	}break;

	case WM_MOUSEMOVE:
	{
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onMouseMove)
			if (func != nullptr)
				func(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;

	case WM_LBUTTONDOWN:
	{
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onMouseDownL)
			if (func != nullptr)
				func(); 
	}break;

	case WM_LBUTTONUP:
	{
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onMouseUpL)
			if (func != nullptr)
				func();
	}break;

	case WM_RBUTTONDOWN:
	{
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onMouseDownR)
			if (func != nullptr)
				func();
	}break;

	case WM_RBUTTONUP:
	{
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onMouseUpR)
			if (func != nullptr)
				func();
	}break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);
			std::cout << "Alt tab triggered!" << " " 
				<< rect.right - rect.left
				<< rect.bottom - rect.top;
		}
		break;

	case WM_KEYDOWN: {
		if (wParam == VK_RETURN && (GetAsyncKeyState(VK_MENU) & 0x8000))

			std::cout << "alt tab!";
		for (auto& func : CoreWindow::WindowHandle->EventHandler->onKeyPressed)
			if (func != nullptr)
				func(wParam);

		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
	}break;

	case WM_SIZING: {
		RECT* rect = reinterpret_cast<RECT*>(lParam);
		
		hWnd = CoreWindow::WindowHandle->GetHWND();
		if (rect)
			for (auto func : CoreWindow::WindowHandle->EventHandler->onWindowSizeChanged)
				if (func != nullptr)
					func(rect->right - rect->left,
						 rect->bottom - rect->top);
	}break;


	case WM_DESTROY:
	{
		PostQuitMessage(0);
		FreeConsole();
		return 0;
	}break;

	default:DefWindowProc(hWnd, Message, wParam, lParam); 
	}
}
