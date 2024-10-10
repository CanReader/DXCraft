#include "Direct11.h"

Direct11::Direct11()
{
}

bool Direct11::SetDepthStencil()
{
	HRESULT result;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = Width;
	depthBufferDesc.Height = Height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = Count;
	depthBufferDesc.SampleDesc.Quality = Quality;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = dev->CreateTexture2D(&depthBufferDesc, nullptr, &DepthBuffer);

	END(result)

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = dev->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);

	END(result)

	// Set the depth stencil state.
	devcon->OMSetDepthStencilState(DepthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = dev->CreateDepthStencilView(DepthBuffer, &depthStencilViewDesc, &DepthStencilView);

	END(result)

	return true;

}

bool Direct11::SetRasterizer()
{
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT result = dev->CreateRasterizerState(&rasterDesc, &RasterState);

	ENDSTR(result, "Failed to create rasterizer state!");

	// Now set the rasterizer state.
	devcon->RSSetState(RasterState);

	return true;
}

bool Direct11::EnumFactory()
{
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&factory);

	END(hr);

	hr = factory->EnumAdapters1(0,&adapter);

	END(hr)

	DXGI_ADAPTER_DESC adapterDesc;
	hr = adapter->GetDesc(&adapterDesc);
	
	END(hr)

	DX_INFO("The adapter vendor: " + adapterDesc.VendorId);
	/* Nvidia: 0x10DE AMD: 0x1002, 0x1022 Intel: 0x163C, 0x8086, 0x8087 */

	factory->Release();

	return true;
}

void Direct11::GetSystemDesc()
{
	VideoMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	SystemMemory = (int)(adapterDesc.DedicatedSystemMemory / 1024 / 1024);
	SharedMemory = (int)(adapterDesc.SharedSystemMemory / 1024 / 1024);
	VideoCardDescription = (char*)adapterDesc.Description;
}

void Direct11::SetProjection(float width, float height)
{
	fov = (80.0f / 360.0f) * XM_2PI;
	ScreenAspect = (float)Width / (float)Height;

	proj = XMMatrixPerspectiveFovLH(fov,ScreenAspect,0.01f,100.0f);
}

Direct11::~Direct11()
{
}

bool Direct11::InitializeDirect3D(HWND hWnd, int Width, int Height, bool FullScreen)
{
#pragma warning(disable:4996)

	if (!EnumFactory())
		return false;
	
	RECT rect;
	GetClientRect(hWnd, &rect);

	this->Width = rect.right - rect.left;
	this->Height = rect.bottom - rect.top;

	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	
	int TotalFeatureLevels = ARRAYSIZE(FeatureLevels);
	
	DXGI_SWAP_CHAIN_DESC sdc;

	//Build structure of swap chain desc
	ZeroMemory(&sdc, sizeof(sdc));
	sdc.BufferCount = 1;
	sdc.BufferDesc.Width = this->Width;
	sdc.BufferDesc.Height = this->Height;
	sdc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sdc.BufferDesc.RefreshRate.Numerator = 60;
	sdc.BufferDesc.RefreshRate.Denominator = 1;
	sdc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	sdc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sdc.OutputWindow = hWnd;
	sdc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	sdc.SampleDesc.Count = Count;
	sdc.SampleDesc.Quality = Quality;
	sdc.Windowed = !FullScreen;

	SetFullScreen(FullScreen);

	//Create swap chain and a device
	HRESULT hr = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT, 0, NULL, D3D11_SDK_VERSION, &sdc, &this->sc, &this->dev, NULL, &this->devcon);

	END(hr)

	DX_INFO("Swap chain and device created!");

	//Get back buffer
	hr = sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);

	END(hr)

	//Create render target view 
	hr = dev->CreateRenderTargetView(BackBuffer, 0, &rtv);

	END(hr)

	if (!SetDepthStencil())
		DX_ERROR("Could not set depth stencil!");

	//Set the render target
	devcon->OMSetRenderTargets(1, &rtv, DepthStencilView);

	//Release the back 
	if (BackBuffer)
		BackBuffer->Release();

	UpdateViewport(Width,Height);

	SetRasterizer();

	SetProjection(this->Width,this->Height);

	GetSystemDesc();

	dev->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&mQueue);

	RendererHelper::SetDevice(dev);
	RendererHelper::SetDeviceContext(devcon);
	RendererHelper::SetAdapter(adapter);

	return true;
}

void Direct11::BeginScene(float R, float G, float B, float A)
{
	float Color[4] = { R,G,B,A };

	devcon->ClearRenderTargetView(rtv, Color);
	devcon->ClearDepthStencilView(DepthStencilView,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);

}

void Direct11::EndScene()
{
	sc->Present((int)Vsync, NULL);
}

void Direct11::DeleteDirect3D()
{
	SAFE_RELEASE(adapter)
	SAFE_RELEASE(sc)
	SAFE_RELEASE(rtv)
	SAFE_RELEASE(DepthBuffer)
	SAFE_RELEASE(DepthStencilState)
	SAFE_RELEASE(DepthStencilView)
	SAFE_RELEASE(RasterState)
	SAFE_RELEASE(devcon)
	SAFE_RELEASE(dev);
	SAFE_RELEASE(mQueue);
}

void Direct11::SetFullScreen(bool FullScreen)
{
	this->FullScreen = FullScreen;
	if (sc)
		sc->SetFullscreenState(FullScreen, NULL);
}

void Direct11::UpdateViewport(float width, float height)
{
	D3D11_VIEWPORT vp;

	this->Width = width;
	this->Height = Height;

	vp.Width = width;
	vp.Height = height;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	devcon->RSSetViewports(1, &vp);
}

XMMATRIX& Direct11::GetProjection()
{
	return proj;
}

HRESULT Direct11::GetDXMessages()
{
	SIZE_T MessageLength = 0;
	UINT64 TotalMessages = mQueue->GetNumStoredMessages();

	for (int i = 0; i < TotalMessages; i++)
	{
	 HRESULT hr = mQueue->GetMessage(i,NULL,&MessageLength);

	  if (FAILED(hr))
		  return hr;

	  D3D11_MESSAGE* Message = (D3D11_MESSAGE*)malloc(MessageLength);
	  mQueue->GetMessage(i,Message,&MessageLength);

	  if (FAILED(hr))
		  return hr;
	  
	  if (messages.size() > 8)
	  {
		  if (messages[messages.size()-1].pDescription != Message->pDescription && 
			  messages[messages.size() - 1].Category != Message->Category)
			  messages.push_back(*Message);
		  else
			  return S_FALSE;
	  }
	  else
		  messages.push_back(*Message);


	  PrintDXMessage(*Message);
	
	  free(Message);
	}

	mQueue->ClearStoredMessages();

	return S_OK;
}

void Direct11::PrintDXMessage(D3D11_MESSAGE message)
{
	auto EnumToStr = [&](D3D11_MESSAGE_SEVERITY& v) 
	{
		switch (static_cast<int>(v))
		{
		case 0:return "CORRUPTION"; break;
		case 1:return "ERROR"; break;
		case 2:return "WARNING"; break;
		case 3:return "INFO"; break;
		case 4:return "MESSAGE"; break;
		default:return " "; break;
		}
	};

	/*
	D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED	
	D3D11_MESSAGE_CATEGORY_MISCELLANEOUS	    
	D3D11_MESSAGE_CATEGORY_INITIALIZATION	    
	D3D11_MESSAGE_CATEGORY_CLEANUP	            
	D3D11_MESSAGE_CATEGORY_COMPILATION	        
	D3D11_MESSAGE_CATEGORY_STATE_CREATION	    
	D3D11_MESSAGE_CATEGORY_STATE_SETTING	    
	D3D11_MESSAGE_CATEGORY_STATE_GETTING	    
	D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION
	D3D11_MESSAGE_CATEGORY_EXECUTION	        	
	*/

	const char* severity = EnumToStr(message.Severity);

	std::cout << severity << ": " << "[ " << message.ID << "] " << message.Category << " " << message.pDescription << "\n";
}
