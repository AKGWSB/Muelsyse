#pragma once

#include "Buffer.h"
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
};

class Mesh
{
public:
	std::string name;
	std::vector<Vertex> vertices;
	std::unique_ptr<Buffer> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Mesh(std::string filepath);
	Mesh();
	~Mesh();
	void GenerateTriangle();
	void GenerateQuad();
	void Upload();
	void Draw();
	void LoadObj(std::string filepath);

	// global resource pool, find by filename
	static std::map<std::string, std::unique_ptr<Mesh>> g_meshResourceMap;
	static Mesh* Find(std::string filepath);
	static void FreeAll();
};
