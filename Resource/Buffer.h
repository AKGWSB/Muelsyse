#pragma once

#include <vector>
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class Buffer
{
public:
	UINT bufferSize;
	ComPtr<ID3D12Resource> buffer;
	std::vector<BYTE> data;

	// for upload
	ComPtr<ID3D12Resource> tempUploadBuffer;
	ComPtr<ID3D12CommandAllocator> tempCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> tempCommandList;
	ComPtr<ID3D12CommandQueue> tempCommandQueue;

	Buffer();
	~Buffer();
	void CreateBuffer(UINT size);
	void UpdateSubData(UINT start, UINT size, void* data);
	void Upload();
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress();
};
