#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

class DefaultBuffer
{
public:
	ComPtr<ID3D12Device> device;
	UINT bufferSize;
	std::wstring bufferName;
	ComPtr<ID3D12Resource> buffer;

	DefaultBuffer(ComPtr<ID3D12Device> device, std::wstring bufferName = L"buffer");
	~DefaultBuffer();

	void UploadData(void* data, UINT size);
};
