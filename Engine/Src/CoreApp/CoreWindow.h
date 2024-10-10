#ifndef COREWINDOW_H_ 
#define COREWINDOW_H_
#define WIN32_LEAN_AND_MEAN

#define DEFAULT_LOCATION 0

#include <Windows.h>
#include <iostream>
#include <memory>
#include <sstream>


#include "Direct11.h"
#include "Graphics.h"
#include "../Debugging/Log.h"
#include "../Debugging/Debugger.h"
#include "EventManager.h"

std::shared_ptr<spdlog::logger> Messanger::Log::m_Logger;


typedef struct Location
{
	int X;
	int Y;
};

typedef struct Size
{
	int Width;
	int Height;
};

class DLL_API CoreWindow
{
public:
	CoreWindow(HINSTANCE hInstance);
	 ~CoreWindow();

	//Variables
private:
	Location Loc;
	Size Siz;

	bool Visible;
	bool Windowed = true;

	LPCSTR ClassName;
	LPCSTR WindowName;

	HMENU Menu;

	MSG Messanger;

	HWND hWnd;
	HINSTANCE hInstance;
public:
	int ReturnValue = 0;

public:
	bool InitializeWindow(LPCSTR WindowName, int Width, int Height);
	void DisplayWindow(bool Show);
	void ProcessMessages();
	void DestroyWindow();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	bool isinShow();

	void SetWindowTitle(const LPSTR&);
	void SetLocation(int X, int Y);
	RECT GetLocation();
	void SetSize(int Width,int Height);
	RECT GetSize();
	void SetFullScreen(bool);


	HWND GetHWND();
	HINSTANCE GetHinstance();

	static CoreWindow* getWindowHandle() { return WindowHandle; }

private:
	WNDCLASSEX CreateWindowClass(LPCSTR ClassName, LPCSTR MenuName);

	inline static CoreWindow* WindowHandle;

public:
	std::unique_ptr<Graphics> graph;
	EventManager* EventHandler;
};

#endif
