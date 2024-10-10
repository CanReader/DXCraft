#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <vector>
#include <memory>
#include <wrl.h>
#include "../Object.h"

class Skybox
{
public:
	Skybox(int LatLines, int LongLines, XMMATRIX* View, XMMATRIX* Proj);
	~Skybox();

	void Render();
	void Update(float DetltaTime);	

private:
	int NumVertices,NumFaces;

	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vs_buffer;
	Microsoft::WRL::ComPtr<ID3D10Blob> ps_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SkyboxTexture;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	ID3D11DeviceContext* devcon;
	ID3D11Device* dev;

	XMMATRIX WorldMatrix;
	XMMATRIX* View;
	XMMATRIX* Proj;
	XMMATRIX matrices;
	Object::ConstantBuffer_WMP wmp;

	XMMATRIX RotationX;
	XMMATRIX RotationY;
	XMMATRIX RotationZ;

	XMMATRIX ScaleMat;
	XMMATRIX TranslationMat;
};

