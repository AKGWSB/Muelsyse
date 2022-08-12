#include "Mesh.h"
#include "../Core/GraphicContex.h"
#include "../Core/helper.h"

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

void Mesh::Draw()
{
    ID3D12GraphicsCommandList* cmdList = GraphicContex::GetInstance()->GetCommandList();

    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    cmdList->DrawInstanced(m_triangleNum, 1, 0, 0);
}
