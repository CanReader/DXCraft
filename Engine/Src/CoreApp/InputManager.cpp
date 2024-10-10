#include "InputManager.h"

InputManager* InputManager::Manager = nullptr;

InputManager::InputManager()
{
}

bool InputManager::InitializeInput(HINSTANCE hInstance, HWND hWnd)
{
    if (!hWnd || !hInstance) {
        DX_ERROR("Invalid HWND or HINSTANCE provided.");
        return false;
    }

    if (FAILED(CoInitialize(NULL))) {
        DX_INFO("Failed to initialize COM object!");
        return false;
    }

    HRESULT hr = DirectInput8Create(
        hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void**>(DirectInput.GetAddressOf()),
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to initialize DirectInput");
        return false;
    }

    hr = DirectInput->CreateDevice(
        GUID_SysKeyboard,
        KeyboardDevice.GetAddressOf(),
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to create keyboard device");
        return false;
    }

    hr = KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    hr = KeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    if (FAILED(hr)) {
        return false;
    }

    // Initialize the mouse
    hr = DirectInput->CreateDevice(
        GUID_SysMouse,
        MouseDevice.GetAddressOf(),
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to create mouse device");
        return false;
    }

    hr = MouseDevice->SetDataFormat(&c_dfDIMouse);
    hr = MouseDevice->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void InputManager::Update()
{
}

void InputManager::Shutdown()
{
    DirectInput.Get()->Release();
    KeyboardDevice.Get()->Unacquire();
    MouseDevice.Get()->Unacquire();
}

DIMOUSESTATE InputManager::GetMouseState()
{
    DIMOUSESTATE mouseState;

    if (MouseDevice == nullptr)
        return {};

    MouseDevice->Acquire();

    MouseDevice->GetDeviceState(sizeof(mouseState),(void**)&mouseState);
    
    return mouseState;
}

bool InputManager::IsKeyPressed(BYTE key)
{
    BYTE keys[256];

    if (KeyboardDevice == nullptr)
        return false;

    KeyboardDevice->GetDeviceState(sizeof(keys), (void**)&keys);

    return (keys[key] & 0x80);
}
