#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <Windows.h>
#include <d3d11.h>
#include <xnamath.h>

typedef XMFLOAT2 Vector2;
typedef XMFLOAT3 Vector3;
typedef XMFLOAT4 Vector4;
typedef XMVECTOR Vector;

typedef XMMATRIX Matrix;

class Object
{
public:
	Object();

	virtual bool Initialize() = 0;
	virtual void Render() = 0;
	virtual void Update(float t) = 0;
	virtual ~Object() {}

	const char* GetName();

protected:
	char* Name;
	XMMATRIX* view;

public:
	struct ConstantBuffer_WMP
	{
		ConstantBuffer_WMP() : WMP(XMMatrixIdentity()) {}

		XMMATRIX WMP;
	};

	struct Vertex
{
public:
	Vertex() = default;
	Vertex(Vector3 pos)
	{
		this->pos = pos;
	}

	Vertex(float x, float y, float z)
	{
		this->pos = Vector3(x, y, z);
		this->col = XMFLOAT4(1, 0, 1, 1);
	}

	Vertex(Vector3 pos, XMFLOAT4 col)
	{
		this->pos = pos;
		this->col = col;
	}

	Vertex(float x, float y, float z, float r, float g, float b, float a)
	{
		pos = Vector3(x, y, z);
		col = Vector4(r, g, b, a);
	}

	Vertex(float x, float y, float z, float u, float v)
	{
		pos = Vector3(x, y, z);
		TextureUV = Vector2(u, v);
	}

	Vector3 pos;
	Vector2 TextureUV;
	Vector4 col;
};
};


#endif // !_OBJECT_H_