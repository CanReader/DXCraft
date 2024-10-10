#pragma once

#include <dinput.h>
#include "../Debugging/Log.h"
#include <wrl.h>


class InputManager
{
friend class Graphics;
public:
	bool IsKeyPressed(BYTE key);
private:
	InputManager();

	bool InitializeInput(HINSTANCE hInstance, HWND hWnd);
	void Update();
	void Shutdown();

	DIMOUSESTATE GetMouseState();

	Microsoft::WRL::ComPtr<IDirectInput8> DirectInput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> KeyboardDevice;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> MouseDevice;
	DIMOUSESTATE mouseCurrentState;
	DIMOUSESTATE mouseLastState;

	static InputManager* Manager;
public:
	static InputManager* GetManager() 
	{
		if (Manager == nullptr)
			Manager = new InputManager();
		return Manager;
	}
};

