#pragma once

#include <vector>
#include <wrl.h>

#include "../Core/DescriptorManager.h"
#include "../Core/d3dx12.h"

using Microsoft::WRL::ComPtr;

#define CBUFFER_BYTE_ALIGN 256

class UploadBuffer
{
private:
	UINT m_size;
	Descriptor m_cbvDescriptor;
	std::vector<BYTE> m_bufferData;
	ComPtr<ID3D12Resource> m_buffer;
	void* m_mappedGpuAddress;

public:
	UploadBuffer(UINT size = 4096);
	~UploadBuffer();

	void UpdateSubData(UINT startByte, UINT size, void* pData);
	void Upload();

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle();
};


