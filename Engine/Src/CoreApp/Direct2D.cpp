#include "Direct2D.h"
#include "Renderer.h"
#include "../CoreGame/Object.h"

Direct2D::~Direct2D()
{
}

Direct2D::Direct2D(HWND hWnd, int Width, int Height) : 
	hWnd(hWnd), 
	Width(Width), 
	Height(Height), 
	dev11(RendererHelper::GetDevice()), 
	dev11con(RendererHelper::GetDeviceContext()), 
	adapter(RendererHelper::GetAdapter())
{
	const char* text = "Hello World";
	int len = strlen(text);
	int wlen = MultiByteToWideChar(CP_UTF8, 0, text, len, NULL, 0);
	wchar_t* wideText = new wchar_t[wlen];

	MultiByteToWideChar(CP_UTF8, 0, text, len, NULL, 0);
}

bool Direct2D::InitializeDirect2D(IDXGIAdapter1* adapter)
{
	HRESULT hr = S_OK;

	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &dev10);

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = dev11->CreateTexture2D(&sharedTexDesc, NULL, &sharedTexture);

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&mutex11);

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource* sharedResource10;
	HANDLE sharedHandle10;

	hr = mutex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1* sharedSurface10;

	hr = dev10->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));

	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&mutex10);

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory* D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &target);

	sharedSurface10->Release();
	D2DFactory->Release();

	// Create a solid color brush to draw something with        
	hr = target->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &brush);

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(wFactory.GetAddressOf()));

	hr = wFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&format
	);

	hr = format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	dev10->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	return InitializeDirect3D10(hWnd,Width,Height);
}

bool Direct2D::CreateText(char* text)
{
	

	return true;

}

bool Direct2D::InitializeDirect3D10(HWND hWnd, int Width, int Height)
{
	Object::Vertex v[] =
	{
		Object::Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Object::Vertex(-1.0f,  +1.0f, -1.0f, 0.0f, 0.0f),
		Object::Vertex(+1.0f,  +1.0f, -1.0f, 1.0f, 0.0f),
		Object::Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	dev11->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Object::Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	HRESULT hr = dev11->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

	dev11->CreateShaderResourceView(sharedTexture.Get(), NULL, view.GetAddressOf());


	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = dev11->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	dev11->CreateBlendState(&blendDesc, state.GetAddressOf());

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = dev11->CreateRasterizerState(&cmdesc, rasterizer.GetAddressOf());

	cmdesc.FrontCounterClockwise = false;

	hr = dev11->CreateRasterizerState(&cmdesc, rasterizer.GetAddressOf());

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = 2 * sizeof(constantBuffer);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = dev11->CreateBuffer(&cbbd, NULL, constantBuffer.GetAddressOf());

	ENDSTR(hr, "Failed to create constant buffer!")

	return true;
}


void Direct2D::Render()
{
	if (!dev10 || !dev11 || !dev11con || !mutex11 || !mutex10)
	{
		DX_ERROR("One or more argument is null!");
		return;
	}

	HRESULT hr = S_OK;

	mutex11->ReleaseSync(0);

	mutex10->AcquireSync(0,5);

	target->BeginDraw();

	target->Clear(D2D1::ColorF(0,0,0,0));

	brush->SetColor(D2D1::ColorF(1,1,1,1));

	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	std::wstring str = L"Hello World!";

	target->DrawText(str.c_str(),
		wcslen(str.c_str()),
		format.Get(),
		layoutRect,
		brush.Get());

	hr = target->EndDraw();

	if (FAILED(hr))
	{
		DX_ERROR("Failed to draw text!!");
	}

	mutex10->ReleaseSync(1);

	mutex11->AcquireSync(1,5);

	dev11con->OMSetBlendState(state.Get(), NULL, 0xffffffff);

	dev11con->IASetIndexBuffer(indexBuffer.Get(),DXGI_FORMAT_R32_UINT,0);

	UINT stride = sizeof(Object::Vertex);
	UINT offset = 0;
	dev11con->IASetVertexBuffers(0,1,vertexBuffer.GetAddressOf(),&stride,&offset);


	dev11con->DrawIndexed(6,0,0);
}
