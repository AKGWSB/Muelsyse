#pragma once

#include <memory>
#include <wrl.h>

#include "../Core/DescriptorManager.h"
#include "../Core/d3dx12.h"
#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

struct Vertex
{
	Vector3 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector3 tangent;
};

class Mesh
{
private:
	UINT m_triangleNum = 0;
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	void LoadFromData(std::vector<Vertex>& vertices);

public:
	Mesh();
	~Mesh();

	void Draw();
};


