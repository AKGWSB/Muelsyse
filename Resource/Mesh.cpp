#include "Mesh.h"
#include "../Core/GraphicContex.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Library/tiny_obj_loader.h"

Mesh::Mesh(std::string filepath)
{
	name = filepath;
	LoadObj(filepath);
	Upload();
}

Mesh::Mesh()
{
	vertexBuffer = std::make_unique<Buffer>();
}

Mesh::~Mesh()
{

}

void Mesh::GenerateTriangle()
{
    vertices = {
        { { 0.0f, 0.25f, 0.0f }, { 0.5f, 1.0f}, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 0.25f, -0.25f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } }
    };

    Upload();
}

void Mesh::GenerateQuad()
{
    vertices = {
        // triangle 1
        { { -1, 1, 0.0f }, { 0.0f, 1.0f}, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, -1, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { -1, -1, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },

        // triangle 2
        { { -1, 1, 0.0f }, { 0.0f, 1.0f}, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, 1, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, -1, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } }
    };

    Upload();
}

void Mesh::Upload()
{
    const UINT vertexBufferSize = sizeof(Vertex) * vertices.size();

    vertexBuffer = std::make_unique<Buffer>();
    vertexBuffer->CreateBuffer(vertexBufferSize);
    vertexBuffer->UpdateSubData(0, vertexBufferSize, vertices.data());
    vertexBuffer->Upload();

    // Initialize the vertex buffer view.
    vertexBufferView.BufferLocation = vertexBuffer->GetGpuAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;
}

void Mesh::Draw()
{
    GraphicContex::g_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GraphicContex::g_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    GraphicContex::g_commandList->DrawInstanced(vertices.size(), 1, 0, 0);
}

void Mesh::LoadObj(std::string filepath)
{
	std::string inputfile = filepath;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {
		//std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		//std::cerr << err << std::endl;
		throw std::exception();
	}

	if (!ret) {
		exit(1);
	}

	std::vector<UINT> tempIdx;

	double INF = 1145141919.810f;
	XMFLOAT3 maxCorner = XMFLOAT3(-INF, -INF, -INF);
	XMFLOAT3 minCorner = XMFLOAT3(+INF, +INF, +INF);

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {

				Vertex vert;

				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				tempIdx.push_back(idx.vertex_index);

				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				maxCorner.x = max(vx, maxCorner.x); maxCorner.y = max(vy, maxCorner.y); maxCorner.z = max(vz, maxCorner.z);
				minCorner.x = min(vx, minCorner.x); minCorner.y = min(vy, minCorner.y); minCorner.z = min(vz, minCorner.z);

				vert.position = XMFLOAT3(vx, vy, vz);

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

					vert.normal = XMFLOAT3(nx, ny, nz);
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

					vert.texcoord = XMFLOAT2(tx, ty);
				}

				vertices.push_back(vert);
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}

	// normalize to
	double maxAxis = max(maxCorner.x - minCorner.x, max(maxCorner.y - minCorner.y, maxCorner.z - minCorner.z));
	XMFLOAT3 center = XMFLOAT3((maxCorner.x + minCorner.x) / 2, (maxCorner.y + minCorner.y) / 2, (maxCorner.z + minCorner.z) / 2);
	for (auto& vert : vertices)
	{
		vert.position.x -= center.x; vert.position.y -= center.y; vert.position.z -= center.z;
		vert.position.x /= maxAxis; vert.position.y /= maxAxis; vert.position.z /= maxAxis;
	}
}

// global resource pool, find by filename
std::map<std::string, std::unique_ptr<Mesh>> Mesh::g_meshResourceMap;
Mesh* Mesh::Find(std::string filepath)
{
	// build in mesh
	if (filepath == "BUILD_IN_QUAD")
	{
		g_meshResourceMap[filepath] = std::make_unique<Mesh>();
		g_meshResourceMap[filepath]->GenerateQuad();
		return g_meshResourceMap[filepath].get();
	}

	// from file
    if (g_meshResourceMap.find(filepath) == g_meshResourceMap.end())
    {
        g_meshResourceMap[filepath] = std::make_unique<Mesh>(filepath);
    }
    return g_meshResourceMap[filepath].get();
}

void Mesh::FreeAll()
{
	for (auto& p : g_meshResourceMap)
	{
		delete p.second.release();
	}
}