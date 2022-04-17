#pragma once

#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

#include "DefaultBuffer.h"

class VertexBuffer : public DefaultBuffer
{
public:
	UINT elementSize;
	D3D12_VERTEX_BUFFER_VIEW bufferView;

	VertexBuffer(ID3D12Device* device, std::wstring bufferName = L"buffer") :
		DefaultBuffer(device, bufferName)
	{

	}

	~VertexBuffer()
	{

	}

	template<typename T>
	void UploadVertexData(const std::vector<T>& data)
	{
		elementSize = static_cast<UINT>(sizeof(T));
		bufferSize = static_cast<UINT>(data.size() * elementSize);

		// send data
		DefaultBuffer::UploadData(data.data(), bufferSize);

		// set buffer view
		bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
		bufferView.StrideInBytes = elementSize;
		bufferView.SizeInBytes = bufferSize;
	}
};
