#include "Player.h"
#include "../../CoreApp/Renderer.h"
#include "../../Debugging/Log.h"

Player::Player()
{
	camera = new Camera(XMVectorSet(6, 14, 6, 0), XMVectorSet(10, 4, 6, 0));
	camera->InitDirectInput(RendererHelper::GetHWND(), RendererHelper::getHINSTANCE());
    position = XMVectorSet(5,0,5,0);
}

Player::~Player()
{
	camera->ResetFocus(true);
	camera->Release();
	delete camera;
}

int totald = 0;
void Player::Update(float deltatime)
{
	camera->Update(deltatime);
    camera->SetPositionRelative(position);
}

void Player::UpdateProj(float pov, int width, int height)
{
	camera->UpdateProj(pov,width,height);
}

void Player::SetSpeed(float speed)
{
    this->speed = speed;
}

float Player::GetSpeed()
{
    return speed;
}

Camera* Player::GetCamera() noexcept
{
	return camera;
}

XMVECTOR Player::GetPositon() noexcept
{
    return XMVECTOR();
}

bool Player::InitializeInput()
{

    if (FAILED(CoInitialize(NULL))) {
        DX_INFO("Failed to initialize COM object!");
        return false;
    }

    HRESULT hr = DirectInput8Create(
        RendererHelper::getHINSTANCE(),
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
    hr = DIKeyboard->SetCooperativeLevel(RendererHelper::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

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
    hr = DIMouse->SetCooperativeLevel(RendererHelper::GetHWND(), DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void Player::MoveForward()
{
}

void Player::MoveBackward()
{
}

void Player::MoveLeft()
{
}

void Player::MoveRigth()
{
}
