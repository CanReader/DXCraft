#include "Camera.h"
#include <dinput.h>
#include "../../Debugging/Log.h"
#include "../../CoreApp/EventManager.h"

Camera::Camera(XMVECTOR Position, XMVECTOR Focus, XMVECTOR Up)
    :
    DefaultForward(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    DefaultRight(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
    Forward(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    UpV(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
    Right(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
    Position(Position),
    Target(Focus),
    Up(Up)
{
    ViewMatrix = XMMatrixLookAtLH(Position, Focus, Up);

    add_event_1(onWindowFocusChanged,&Camera::ResetFocus,this);
}

Camera::~Camera()
{
}

bool Camera::InitDirectInput(HWND hWnd, HINSTANCE hInstance) {
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
        reinterpret_cast<void**>(&DirectInput),
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to initialize DirectInput");
        return false;
    }

    hr = DirectInput->CreateDevice(
        GUID_SysKeyboard,
        &DIKeyboard,
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to create keyboard device");
        return false;
    }

    hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    hr = DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    if (FAILED(hr)) {
        return false;
    }

    // Initialize the mouse
    hr = DirectInput->CreateDevice(
        GUID_SysMouse,
        &DIMouse,
        nullptr
    );

    if (FAILED(hr)) {
        DX_ERROR("Failed to create mouse device");
        return false;
    }

    hr = DIMouse->SetDataFormat(&c_dfDIMouse);
    hr = DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void Camera::Update(float dt)
{
    HandleInput(dt);

    Rotation = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    Target = XMVector3TransformCoord(DefaultForward, Rotation);
    Target = XMVector3Normalize(Target);

    XMMATRIX TempMatrix;
    TempMatrix = XMMatrixRotationY(camYaw);
    Right = XMVector3TransformCoord(DefaultRight, TempMatrix);
    Forward = XMVector3TransformCoord(DefaultForward, TempMatrix);
    Up = XMVector3TransformCoord(Up, TempMatrix);
    

    Position += moveLeftRight * Right;
    Position += moveBackForward * Forward;
    Position += moveUpDown * UpV;

    if (XMVectorGetY(Position) <= 3)
        Position -= moveUpDown * UpV;
    Target = Position + Target;

    ViewMatrix = XMMatrixLookAtLH(Position, Target, Up);
    
    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;
    moveUpDown = 0.0f;

}

void Camera::HandleInput(float dt)
{
    if (DIKeyboard == nullptr || DIMouse == nullptr)
        return;

    if (WindowFocus) {
        DIKeyboard->Acquire();
        DIMouse->Acquire();
    }
    else {
        DIKeyboard->Unacquire();
        DIMouse->Unacquire();
        return;
    }

    DIMOUSESTATE mouseState;

    BYTE keys[256];

    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&mouseState);
    DIKeyboard->GetDeviceState(sizeof(keys), (void**)&keys);

    float speed = 15.0f * dt;

    if (keys[DIK_A] & 0x80)
    {
        moveLeftRight -= speed;
    }
    if (keys[DIK_D] & 0x80)
    {
        moveLeftRight += speed;
    }
    if (keys[DIK_W] & 0x80)
    {
        moveBackForward += speed;
    }
    if (keys[DIK_S] & 0x80)
    {
        moveBackForward -= speed;
    }
    if (keys[DIK_SPACE] & 0x80)
        moveUpDown += speed;

    if (keys[DIK_LSHIFT] & 0x80)
        moveUpDown -= speed;

    if ((mouseState.lX != mouseLastState.lX) || (mouseState.lY != mouseLastState.lY))
    {
        camYaw += mouseLastState.lX * (20.0f/10000);

        camPitch += mouseState.lY * (20.0f/10000);

        if (camPitch > 0.85f)
            camPitch = 0.85;

        if (camPitch < -0.85f)
            camPitch = -0.85;

        mouseLastState = mouseState;
    }
}

void Camera::ResetFocus(bool LostFocus)
{
    this->WindowFocus = !LostFocus;
    if (LostFocus) {
        if (DIKeyboard) {
            DIKeyboard->Unacquire();
        }
        if (DIMouse) {
            DIMouse->Unacquire();
        }
    }

    else {
        if (DIKeyboard) {
            DIKeyboard->Acquire();
        }
        if (DIMouse) {
            DIMouse->Acquire();
        }
    }
}

void Camera::Release()
{
    if(DirectInput)
        DirectInput->Release();
    if(DIKeyboard)
        DIKeyboard->Unacquire();
    if(DIMouse)
        DIMouse->Unacquire();
}

void Camera::UpdateProj(float angle, int width, int height, float nearp, float farp)
{
    if(width - height > 1)
    ProjMatrix = XMMatrixPerspectiveFovLH((angle / 360.0f) * XM_2PI,width/height,nearp,farp);
}

void Camera::UpdateProj(XMMATRIX matrix)
{
    ProjMatrix = matrix;
}

void Camera::SetPosition(XMVECTOR position)
{
    this->Position = position;
}

void Camera::SetPositionRelative(XMVECTOR BasePosition)
{
    XMVectorSet(
        XMVectorGetX(Position)+ XMVectorGetX(BasePosition),
        XMVectorGetY(Position)+ XMVectorGetY(BasePosition),
        XMVectorGetZ(Position)+ XMVectorGetZ(BasePosition),
        1);
    
}

XMVECTOR Camera::GetPosition() 
{
    return XMVectorSet(
        ViewMatrix(3,0),
        ViewMatrix(3,1),
        ViewMatrix(3,2),
        ViewMatrix(3,3));
}

XMMATRIX* Camera::GetView()
{
    return &ViewMatrix;
}

XMMATRIX* Camera::GetProj()
{
    return &ProjMatrix;
}

bool Camera::isPressed(BYTE key)
{
    DIMOUSESTATE mouseState;

    BYTE keys[256];

    if (DIKeyboard == nullptr)
        return false;

    DIKeyboard->GetDeviceState(sizeof(keys), (void**)&keys);

    return (keys[key] & 0x80);
}
