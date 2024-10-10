#pragma once
#include "../Camera/Camera.h"
#include "Crosshair.h"

class Player
{
public:
	Player();
	~Player();
	
	void Update(float deltatime);
	void UpdateProj(float pov, int width, int height);

	void SetSpeed(float speed);
	float GetSpeed();
	
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRigth();

	void RotateCamera(float yaw, float pitch);
	
	void RightClickEvent();
	void LeftClickEvent();
	
	void ChangeBlock(bool decrement = false);
	
	Camera* GetCamera() noexcept;
	void ToggleFlying() noexcept;
	XMVECTOR GetPositon() noexcept;

private:
	bool InitializeInput();

	void Raycast();
	void ResolveCollision(XMVECTOR delta);

	Camera* camera;
	Crosshair* crosshair;

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	IDirectInput8* DirectInput;
	
	XMVECTOR position;

	bool collision = true;
	bool flying = false;
	bool falling = false;
	bool jumping = false;

	float fallVelocity;
	float moveVelocity = 0.0f;

	const float baseSpeed = speed;
	float speed = 5.0;
	float sensivity;

	float jumpVelocity = 0.0f;
	float jumpDistance = 8.75f;

	float jumpSpeedModifier = 5.0f;
	float fallSpeedModifier = 2.75f;

	float fallMinBound = 4.0f;


	const float moveConstant = 0.0125f;

	const float jumpFallConstant = 0.02f;

	const float flyingSpeedModifier = 4.0f;

	const float blockFace = 0.0501f;

	const float diagonalBlockFace = blockFace;

	const float fallingDecreaseConstant = 0.02f;

	const float jumpingDecreaseConstant = 0.75f;
};

