#ifndef _Direct2D_H_
#define _Direct2D_H_

#include "../Debugging/Debugger.h"
#include <wrl.h>
#include <D3D10_1.h>
#include <D3D11.h>
#include <D2D1.h>
#include <DWrite.h>

#include "../CoreGame/Object.h"

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"d3d10_1.lib")
#pragma comment(lib,"dwrite.lib")

using namespace Microsoft::WRL;

/*
* Because DirectWrite can only be used with Directx 10, firstly we need to initialize Directx 10.1.
-----------------------------------------------------------
* After initializing Directx 10, we need to initialize shared texture which d3d10.1 and d3d11 will share this object
-----------------------------------------------------------
* After that keyed mutex is initialized for both versions of Directx, keyed mutex is needed for changing the ownership of the shared texture.
-----------------------------------------------------------
* Since we can't create a keyed mutex directly to this texture from D3D 10.1, since this texture is a D3D 11, we need to create a IDXGIResource object. which we can then get a handle to, then create a shared surface using that handle to that resource, giving D3D 10.1 access to the texture. Basically what we are doing is getting a pointer to this texture, and storing it in an IDXGIResource object.
* IDXGIResource = pointer to texture
-----------------------------------------------------------
* After this, handle to shared resource is needed. Using this handle, we can open the resource to this texture from d3d10.1. in later, We will create an surface object to store the pointer to the shared resource. So whats actually happening is D3D 10.1 will be rendering to this surface, which is connected to the D3D 11 texture.
-----------------------------------------------------------
* Now we have keyed mutexs for the d3d11 texture. we can initialize D2D. We start by ID2D1Factory
* To initialize D2D all we do is set its render target and create brush
-----------------------------------------------------------
* After all these initializations, finally we can initialize our DirectWrite API. DirectWrite tells D2D how to draw text. To initialize we create IDWriteFactory object.
-----------------------------------------------------------
* Every property of text is determined by IDWriteTextFormat object
-----------------------------------------------------------
* Finallly we can start for drawing. we can create a shader resource from the shared texture which D3D 10 and D2D render to, and texture a square in D3D 11 with that shader resource, just like the shader resource we create from an image in a file.
* Because our app works Directx 11, we will create a square in d3d11 and D2D/D3D10 will render text to this square!
-----------------------------------------------------------
* When we created the square, we set the vertices to -1 to 1 for the x and y axis. reset WVP matrix before we render!
-----------------------------------------------------------
* Only one device can use that texture at one time. That is what the keyed mutex cares!
*/

class Direct2D
{
public:
	Direct2D() = delete;
	Direct2D(Direct2D&) = delete;
	Direct2D(Direct2D&&) = delete;
	~Direct2D();
	Direct2D(HWND hWnd, int Width, int Height);


	bool InitializeDirect2D(IDXGIAdapter1*);

	bool CreateText(char* text);
	
	void Render();
private:

	bool InitializeDirect3D10(HWND hWnd, int Width, int Height);
	
private:
	HWND hWnd;
	int Width;
	int Height;

	ComPtr<ID3D11Device> dev11;
	ComPtr<ID3D11DeviceContext> dev11con;
	ComPtr<ID3D10Device1> dev10;
	ComPtr<IDXGIAdapter1> adapter;
	
	ComPtr<IDXGIKeyedMutex> mutex10;
	ComPtr<IDXGIKeyedMutex> mutex11;
	
	ComPtr<ID3D11Texture2D> sharedTexture;
	ComPtr<ID3D11ShaderResourceView> view;
	ComPtr<IDXGISurface1> surface;

	ComPtr<ID2D1RenderTarget> target;
	
	ComPtr<ID2D1Factory> D2factory;
	ComPtr<ID2D1SolidColorBrush> brush;
	ComPtr<IDWriteFactory> wFactory;
	ComPtr<IDWriteTextFormat> format;

	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> constantBuffer;

	ComPtr<ID3D11BlendState> state;
	ComPtr<ID3D11SamplerState> sampler;
	ComPtr<ID3D11RasterizerState> rasterizer;

	Object::ConstantBuffer_WMP wmp;

	std::wstring wText;
};

#endif