#include "Skybox.h"
#include "../../CoreApp/Renderer.h"
#include "../../Debugging/Log.h"

#define SKYBOX_TEXTURE "skybox.dds"

Skybox::Skybox(int LatLines, int LongLines, XMMATRIX* View, XMMATRIX* Proj)
	: View(View), Proj(Proj), ScaleMat(XMMatrixScaling(1000,1000,1000))
{
	dev = RendererHelper::GetDevice();
	devcon = RendererHelper::GetDeviceContext();

	NumVertices = ((LatLines - 2) * LongLines) + 2;
	NumFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Object::Vertex> vertices(NumVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;

	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		RotationX = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			RotationY = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (RotationX * RotationY));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].pos.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].pos.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumVertices - 1].pos.x = 0.0f;
	vertices[NumVertices - 1].pos.y = 0.0f;
	vertices[NumVertices - 1].pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Object::Vertex) * NumVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = dev->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &VertexBuffer);


	std::vector<DWORD> indices(NumFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumVertices - 1;
		indices[k + 1] = (NumVertices - 1) - (l + 1);
		indices[k + 2] = (NumVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumVertices - 1;
	indices[k + 1] = (NumVertices - 1) - LongLines;
	indices[k + 2] = NumVertices  - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	dev->CreateBuffer(&indexBufferDesc, &iinitData, &IndexBuffer);


	/*/////////////////////////////////////
	///Constant Buffer
	*//////////////////////////////////////

	D3D11_BUFFER_DESC ConstantBufferDesc;
	ZeroMemory(&ConstantBufferDesc, sizeof(ConstantBufferDesc));

	ConstantBufferDesc.ByteWidth = sizeof(Object::ConstantBuffer_WMP);
	ConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = 0;
	ConstantBufferDesc.MiscFlags = 0;

	dev->CreateBuffer(&ConstantBufferDesc,nullptr,ConstantBuffer.GetAddressOf());

	/*/////////////////////////////////////
	///Create Shaders
	*//////////////////////////////////////

	
	ID3D10Blob* CompileError;
	D3DX11CompileFromFile("SkyboxShader.hlsl", 0, 0, "VS", "vs_4_0", 0, 0, 0, &vs_buffer, &CompileError, &hr);
	if (FAILED(hr))
	{
		DX_ERROR("Failed to compile vertex shader file for skybox");
		if (CompileError) {
			char* message =  (char*)CompileError->GetBufferPointer();
			DX_ERROR(message)
		}

		return;
	}


	dev->CreateVertexShader(vs_buffer->GetBufferPointer(),vs_buffer->GetBufferSize(),NULL,VertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		DX_ERROR("Failed to create vertex shader for skybox");
		return;
	}


	D3DX11CompileFromFile("SkyboxShader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &ps_buffer, &CompileError, &hr);
	if (FAILED(hr))
	{
		DX_ERROR("Failed to compile vertex shader file for skybox");
		if (CompileError) {
			char* message = (char*)CompileError->GetBufferPointer();
			DX_ERROR(message)
		}

		return;
	}

	dev->CreatePixelShader(ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), NULL, PixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		DX_ERROR("Failed to create pixel shader file for skybox");
		
		return;
	}


	/*/////////////////////////////////////
	///Load Texture
	*//////////////////////////////////////

	D3DX11_IMAGE_LOAD_INFO LoadInfo;
	LoadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* SMTexture = 0;
	hr = D3DX11CreateTextureFromFile(dev,SKYBOX_TEXTURE,&LoadInfo,nullptr,(ID3D11Resource**)&SMTexture, 0);

	if (FAILED(hr))
	{
		DX_ERROR("Failed to create texture for skybox");
		return;
	}

	D3D11_TEXTURE2D_DESC TextureDesc;
	SMTexture->GetDesc(&TextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC ViewDesc;
	ViewDesc.Format = TextureDesc.Format;
	ViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	ViewDesc.TextureCube.MipLevels = TextureDesc.MipLevels;
	ViewDesc.TextureCube.MostDetailedMip = 0;

	hr = dev->CreateShaderResourceView(SMTexture,&ViewDesc, SkyboxTexture.GetAddressOf());

	if (FAILED(hr))
	{
		DX_ERROR("Failed to create resource view for skybox");
		return;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = dev->CreateSamplerState(&samplerDesc, &SamplerState);
	if (FAILED(hr))
	{
		DX_ERROR("Failed to create sampler state!");
		return;
	}

	/*/////////////////////////////////////
	///Set render state
	*//////////////////////////////////////
	D3D11_RASTERIZER_DESC rasteresc;
	rasteresc.CullMode = D3D11_CULL_FRONT;
	rasteresc.FillMode = D3D11_FILL_SOLID;
	hr = dev->CreateRasterizerState(&rasteresc,RasterState.GetAddressOf());
	if (FAILED(hr))
	{
		DX_ERROR("Failed to create rasterizer state for skybox");
		return;
	}

	D3D11_DEPTH_STENCIL_DESC depthdesc;
	ZeroMemory(&depthdesc,sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthdesc.DepthEnable = true;
	depthdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = dev->CreateDepthStencilState(&depthdesc,&DepthStencil);
	if (FAILED(hr))
	{
		DX_ERROR("Failed to create depth stencil for skybox");
		return;
	}
}

Skybox::~Skybox()
{
}

void Skybox::Render()
{
	if (VertexBuffer || IndexBuffer) {
		ID3D11DeviceContext* devcon = RendererHelper::GetDeviceContext();

		UINT stride = sizeof(Object::Vertex);
		UINT offset = 0;

		devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devcon->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		devcon->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);
		matrices = WorldMatrix * (*View) * (*Proj);
		wmp.WMP = XMMatrixTranspose(matrices);
		devcon->UpdateSubresource(ConstantBuffer.Get(), 0, NULL, &wmp, 0, 0);
		devcon->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
		devcon->PSSetShaderResources(0, 1, SkyboxTexture.GetAddressOf());
		devcon->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
		devcon->VSSetShader(VertexShader.Get(), 0, 0);
		devcon->PSSetShader(PixelShader.Get(), 0, 0);
		devcon->OMSetDepthStencilState(DepthStencil.Get(), 0);
		devcon->RSSetState(RasterState.Get());
		devcon->DrawIndexed(NumFaces * 3, 0, 0);
	}
}

void Skybox::Update(float dt)
{
	WorldMatrix = XMMatrixIdentity();

	auto CamPos = XMVectorSet(
		-(*View)(3, 0),
		-(*View)(3, 1),
		-(*View)(3, 2),
		1);
	
	TranslationMat = XMMatrixTranslation(
	XMVectorGetX(CamPos),
	XMVectorGetY(CamPos),
	XMVectorGetZ(CamPos)
	);

	WorldMatrix = ScaleMat * TranslationMat;
}
