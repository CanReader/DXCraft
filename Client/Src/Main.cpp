#include "CoreApp/CoreWindow.h"

using namespace std;

void CreateConsole() {
    AllocConsole();

    FILE* fp;

    // Redirect stdout
    freopen_s(&fp, "CONOUT$", "w", stdout);
    // Redirect stderr
    freopen_s(&fp, "CONOUT$", "w", stderr);
    // Redirect stdin
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	int ReturnValue = 0;

	CreateConsole();

	CoreWindow* Window = new CoreWindow(hInstance);
	if (Window->InitializeWindow("DirectX App",1270,720))
	{
		Window->ProcessMessages();
		ReturnValue = Window->ReturnValue;
	}
	else
		MessageBox(NULL,"Failed to create window!","ERROR",MB_OK | MB_ICONERROR);

	delete Window;
	Window = 0;

	return ReturnValue;
}
