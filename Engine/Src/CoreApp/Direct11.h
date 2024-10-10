#ifndef D3D_H_
#define D3D_H_

#include <d3d11.h>
#include <DXGI.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <D3DX10math.h>
#include <d3dcompiler.h>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <wrl.h>

#include "Direct2D.h"
#include "Renderer.h"

#pragma comment(lib,"dxgi")

class Direct11
{
//Variabless

public:
	int Width; 
	int Height;

	UINT Count = 1;
	UINT Quality = 0;
	
	bool Windowed;
	bool Vsync = true;
	bool FullScreen;

	int VideoMemory;
	int SystemMemory;
	int SharedMemory;
	char* VideoCardDescription;

	float fov;
	float Near = 0.1f;
	float Far = 100.0f;
	float ScreenAspect;

	private:
	D3D_FEATURE_LEVEL* flevel;
	DXGI_FRAME_STATISTICS fs;
	DXGI_ADAPTER_DESC adapterDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	IDXGIAdapter1* adapter;
	IDXGISwapChain* sc;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> SharedTexture;
	ID3D11Texture2D* DepthBuffer;
	ID3D11DepthStencilState* DepthStencilState;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RasterizerState* RasterState;

	ID3D11Texture3D* BackBuffer;
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	ID3D11RenderTargetView* rtv;
	IDXGIFactory1* factory;
	
	ID3D11InfoQueue* mQueue;

	ID3D11ShaderResourceView* D2DTexture;
	
	XMMATRIX proj;

	std::vector<D3D11_MESSAGE> messages;

//Methods
public:
	Direct11();
	Direct11(Direct11&) = delete;
	Direct11(Direct11&&) = delete;
	~Direct11();

	bool InitializeDirect3D(HWND hWnd, int Width, int Height, bool FullScreen);
	void BeginScene(float R, float G, float B, float A);
	void EndScene();
	void DeleteDirect3D();
	void SetFullScreen(bool);
	void UpdateViewport(float width, float height);
	
	XMMATRIX& GetProjection();
	void SetProjection(float width, float height);

	HRESULT GetDXMessages();
	void PrintDXMessage(D3D11_MESSAGE message);

private:
	 bool SetDepthStencil();
	 bool SetRasterizer();
	 bool EnumFactory();
	 void GetSystemDesc();

	 Direct2D* d2d;
};

#endif
