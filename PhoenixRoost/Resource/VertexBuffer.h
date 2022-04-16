#pragma once

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

	VertexBuffer(ComPtr<ID3D12Device> device, std::wstring bufferName = L"buffer");
	~VertexBuffer();

	template<typename T>
	void UploadVertexData(const std::vector<T>& data);
};
