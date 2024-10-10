#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <D3D11.h>
#include <xnamath.h>
#include <dinput.h>
#include <DXGI.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Camera
{
public:
	Camera(XMVECTOR Position = XMVectorSet(0, 5, 0, 0),
		XMVECTOR Focus = XMVectorSet(0, 0, 0, 0),
		XMVECTOR Up = XMVectorSet(0, 2, 0, 0));
	~Camera();

	void Update(float dt);

	bool InitDirectInput(HWND hWnd, HINSTANCE hInstance);
	void HandleInput(float dt);

	bool isPressed(BYTE key);
	void ResetFocus(bool LostFocus);
	void Release();

	void UpdateProj(float angle, int width, int height, float near = 1.0f, float far = 1000.0f);
	void UpdateProj(XMMATRIX matrix);

	void SetPosition(XMVECTOR position);
	void SetPositionRelative(XMVECTOR BasePosition);

	XMMATRIX* GetView();
	XMMATRIX* GetProj();
	XMVECTOR GetPosition();

private:
	class ViewFrustum* frustum;

	const XMVECTOR DefaultForward;
	const XMVECTOR DefaultRight;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	IDirectInput8* DirectInput;

	XMVECTOR Position;
	XMVECTOR Target;
	XMVECTOR Up;
	XMVECTOR Forward;
	XMVECTOR Right;
	XMVECTOR UpV;

	XMMATRIX Rotation;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjMatrix;

	bool WindowFocus = true;

	float camYaw;
	float camPitch;

	float moveLeftRight, moveBackForward, moveUpDown;

public:
	class ViewFrustum {
		ViewFrustum() = default;
	};
};

