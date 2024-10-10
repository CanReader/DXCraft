#include <xnamath.h>
#include <d3d11.h>

class VVector
{
public:
	VVector(float x = 0, float y = 0, float z = 0, float w = 0)
		: vector(XMVectorSet(x, y, z, w)) {}
	VVector(XMVECTOR vec)
		: vector(vec) {}

	float GetX() const { return XMVectorGetX(vector); }
	float GetY() const { return XMVectorGetY(vector); }
	float GetZ() const { return XMVectorGetZ(vector); }
	float GetW() const { return XMVectorGetW(vector); }

	inline XMVECTOR Get() const { return vector; }

	inline XMFLOAT3 ToFloat3() const
	{
		return XMFLOAT3(XMVectorGetX(vector), XMVectorGetY(vector), XMVectorGetZ(vector));
	}

	inline XMFLOAT4 ToFloat4() const
	{
		return XMFLOAT4(XMVectorGetX(vector), XMVectorGetY(vector), XMVectorGetZ(vector), XMVectorGetW(vector));
	}

private:
	XMVECTOR vector;
};

