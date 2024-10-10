#ifndef _CUBE_H_
#define _CUBE_H_

#include <D3D11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>	
#include <xnamath.h>
#include "../Object.h"
#include "../../Debugging/Log.h"

#define IndexCount 36

class Cube : public Object
{
public:
	Cube();
	Cube(float x, float y, float z);
	Cube(const char* TexturePath);
	Cube(float x, float y, float z, const char* TexturePath);
	Cube(float x, float y, float z, ID3D11ShaderResourceView* Texture);
	~Cube();

	virtual bool Initialize() override;
	virtual void Render() override;
	virtual void Update(float t) override;

	bool InitializeTexture();
	void PassMatrices(Matrix* view, Matrix* proj);

protected:
	void Shutdown();

private:
	void CompileShader();
	bool CompileColorShader();
	bool CompileTextureShader();

	void SetBlending();

	bool CreateBuffers(D3D11_SUBRESOURCE_DATA, D3D11_SUBRESOURCE_DATA);

	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;

	ID3D11InputLayout* lay;
	
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;

	ID3D11Buffer* WVPBuffer;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	D3D11_INPUT_ELEMENT_DESC* layoutDesc;

	ID3D10Blob* vsBlob;
	ID3D10Blob* psBlob;

	ID3D11BlendState* Transparency;

	ID3D11RasterizerState* Rasterizer;

	//Constant Buffer
	XMMATRIX worldMatrix;
	XMMATRIX* viewMatrix;
	XMMATRIX* projMatrix;

	//Textures
	const char* TexturePath = "";
	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* samplerState;
	ID3D11DepthStencilState* DepthState;

	std::unique_ptr<ConstantBuffer_WMP> wmp;
	
};


#endif // !1