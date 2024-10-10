#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <Windows.h>

#include "Direct11.h"
#include "Renderer.h"
#include "../CoreGame/Game.h"
#include "InputManager.h"


#define Iterate(gameObject) for(auto& i : gameObject)

class Graphics
{
	//Variables
public:
	bool Windowed = false;
	bool vsync = false;
	
	float Screen_Depth = 1000.0f;
	float Screen_Far = 0.1f;

	int Width = 0;
	int Height = 0;
private:
	
	//Methods
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(HWND,HINSTANCE,int,int,bool);
	void Render();
	void Shutdown();

	void UpdateScene(float dt);

	void SetFullscreen(bool);

	void OnWindowSizeChangedEvent(int width, int height);

	inline XMMATRIX& GetProjection() { return d3d->GetProjection(); }
private:
	HWND hWnd;

	void UpdateDeltaTime();

	LARGE_INTEGER frequency;
	LARGE_INTEGER startTime;
	LARGE_INTEGER currentTime;
	double deltaTime;

	//Classes
private:
	Game* game = nullptr;
	Direct11* d3d = nullptr;
	InputManager* input;
};

#endif