#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

#include "DefaultBuffer.h"


class IndexBuffer : public DefaultBuffer
{
public:
	UINT indexCount;
	D3D12_INDEX_BUFFER_VIEW bufferView;

	IndexBuffer(ComPtr<ID3D12Device> device, std::wstring bufferName = L"buffer") :
		DefaultBuffer(device, bufferName)
	{

	}

	~IndexBuffer()
	{

	}

	void UploadIndexData(const std::vector<UINT>& indices)
	{
		indexCount = indices.size();
		bufferSize = static_cast<UINT>(indexCount * sizeof(UINT));

		// send data
		DefaultBuffer::UploadData(indices.data(), bufferSize);

		// set buffer view
		bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
		bufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
		bufferView.SizeInBytes = bufferSize;
	}
};
