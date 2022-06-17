#pragma once

#include <vector>
#include <map>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "../Core/d3dx12.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class UploadBuffer
{
public:
	UINT bufferSize;
	ComPtr<ID3D12Resource> buffer;
	std::vector<BYTE> data;		// data in cpu

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle;		// cpu handle for cbv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGpuHandle;		// gpu handle for cbv descriptor
	UINT cbvHandleIndex;							// index in cbv descriptor heap

	UploadBuffer();
	~UploadBuffer();
	void CreateBuffer(UINT size);
	void Upload();
	void UpdateSubData(UINT start, UINT size, void* src);
};
