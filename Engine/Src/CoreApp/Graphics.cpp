#include "Graphics.h"
#include "Renderer.h"
#include <Windows.h>
#include "EventManager.h"
#include <functional>

Graphics::Graphics() {}

Graphics::Graphics(const Graphics&) {}

Graphics::~Graphics() {}

bool Graphics::Initialize(HWND hWnd, HINSTANCE hInstance, int Width, int Height, bool FullScreen)
{
	this->Windowed = FullScreen;
	this->Width = Width;
	this->Height = Height;
	this->hWnd = hWnd;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);

	d3d = new Direct11();

	if (!d3d->InitializeDirect3D(hWnd, Width, Height, FullScreen))
		return false;

	input = InputManager::GetManager();
	input->InitializeInput(hInstance,hWnd);

	DX_INFO("Direct3d is initialized!");

	RendererHelper::SetHWND(&hWnd);
	RendererHelper::SetHINSTANCE(&hInstance);

	game = new Game(XMFLOAT2(Width,Height),&GetProjection());

	DX_INFO("The game has been initialized!");
	
	add_event_2(onWindowSizeChanged,&Graphics::OnWindowSizeChangedEvent,this);

	return true;
}

void Graphics::Render()
{
	d3d->BeginScene(0.529f, 0.807f, 0.921f, 0);

	UpdateDeltaTime();

	UpdateScene(deltaTime);
	
	game->Render(),
	d3d->EndScene();
}

void Graphics::Shutdown()
{
	input->Shutdown();
	delete input;

	d3d->DeleteDirect3D();
	delete d3d;
	d3d = 0;

	delete game;
}

void Graphics::UpdateScene(float dt)
{
	if (input != nullptr)
		input->Update();

	if (game != nullptr)
		game->Update(dt);
}

void Graphics::SetFullscreen(bool FullScren)
{
	d3d->SetFullScreen(FullScren);
}

void Graphics::OnWindowSizeChangedEvent(int width, int height)
{
}

void Graphics::UpdateDeltaTime()
{
	QueryPerformanceCounter(&currentTime);

	LONGLONG elapsedTicks = currentTime.QuadPart - startTime.QuadPart;
	deltaTime = static_cast<double>(elapsedTicks) / frequency.QuadPart;

	startTime = currentTime;
}
