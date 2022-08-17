#include "Mesh.h"
#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Library/tiny_obj_loader.h"
#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

Mesh::Mesh(std::string filepath)
{
	name = filepath;
	LoadObj(filepath);
	LoadFromData(m_vertices);
}

Mesh::Mesh()
{
	std::vector<Vertex> vertices = {
        // triangle 1
        { { -1, 1, 0.0f }, { 0.0f, 1.0f}, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, -1, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { -1, -1, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },

        // triangle 2
        { { -1, 1, 0.0f }, { 0.0f, 1.0f}, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, 1, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } },
        { { 1, -1, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0, 0.0f }, { 0.0f, 0, 0.0f } }
	};

    LoadFromData(vertices);
}

Mesh::~Mesh()
{

}

void Mesh::LoadFromData(std::vector<Vertex>& vertices)
{
    GraphicContex* contex = GraphicContex::GetInstance();
    ID3D12Device* device = contex->GetDevice();
    ID3D12GraphicsCommandList* cmdList = contex->GetCommandList();

    m_triangleNum = vertices.size();
    UINT bufferSize = m_triangleNum * sizeof(Vertex);


    // create default heap
    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,                // a default heap
        D3D12_HEAP_FLAG_NONE,           // no flags
        &resourceDesc,                  // resource description for a buffer
        D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));
    

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = bufferSize;


    // create upload heap
    ComPtr<ID3D12Resource> tempUploadBuffer;

    auto uHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto uResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    ThrowIfFailed(device->CreateCommittedResource(
        &uHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempUploadBuffer)));


    // upload
    D3D12_SUBRESOURCE_DATA subDataDesc = {};
    subDataDesc.pData = vertices.data();      // pointer to our index array
    subDataDesc.RowPitch = bufferSize;        // size of all our index buffer
    subDataDesc.SlicePitch = bufferSize;      // also the size of our index buffer

    UpdateSubresources(cmdList, m_vertexBuffer.Get(), tempUploadBuffer.Get(), 0, 0, 1, &subDataDesc);


    // change buffer state
    auto to_buffer = CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    cmdList->ResourceBarrier(1, &to_buffer);


    // 
    contex->SyncExecute(cmdList);
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
	Vector3 maxCorner = Vector3(-INF, -INF, -INF);
	Vector3 minCorner = Vector3(+INF, +INF, +INF);

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

				vert.position = Vector3(vx, vy, vz);

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

					vert.normal = Vector3(nx, ny, nz);
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

					vert.texcoord = Vector2(tx, ty);
				}

				m_vertices.push_back(vert);
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}

	// normalize to
	double maxAxis = max(maxCorner.x - minCorner.x, max(maxCorner.y - minCorner.y, maxCorner.z - minCorner.z));
	Vector3 center = Vector3((maxCorner.x + minCorner.x) / 2, (maxCorner.y + minCorner.y) / 2, (maxCorner.z + minCorner.z) / 2);
	for (auto& vert : m_vertices)
	{
		vert.position.x -= center.x; vert.position.y -= center.y; vert.position.z -= center.z;
		vert.position.x /= maxAxis; vert.position.y /= maxAxis; vert.position.z /= maxAxis;
	}
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
    // ID3D12GraphicsCommandList* cmdList = GraphicContex::GetInstance()->GetCommandList();

    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    cmdList->DrawInstanced(m_triangleNum, 1, 0, 0);
}
