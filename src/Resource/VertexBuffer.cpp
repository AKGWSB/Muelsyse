#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(ComPtr<ID3D12Device> device, std::wstring bufferName) :
	DefaultBuffer(device, bufferName)
{
	
}

VertexBuffer::~VertexBuffer()
{

}

template<typename T>
void VertexBuffer::UploadVertexData(const std::vector<T>& data)
{
	elementSize = static_cast<UINT>(sizeof(T));
	bufferSize = static_cast<UINT>(data.size() * elementSize);

	// send data
	void* pData = data.data();
	DefaultBuffer::UploadData(pData, bufferSize);

	// set buffer view
	bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	bufferView.StrideInBytes = elementSize;
	bufferView.SizeInBytes = bufferSize;
}
