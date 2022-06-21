#pragma once

#include "../Core/d3dx12.h"
#include <wrl.h>
#include <string>
#include <map>

using Microsoft::WRL::ComPtr;

class Texture2D
{
public:
	std::string name;
	int width;
	int height;
	DXGI_FORMAT format;

	ComPtr<ID3D12Resource> buffer;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	CD3DX12_CPU_DESCRIPTOR_HANDLE samplerCpuHandle;	// cpu handle for sampler descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle;	// cpu handle for sampler descriptor
	UINT samplerHandleIndex;						// index in sampler descriptor heap

	Texture2D(int _width, int _height, DXGI_FORMAT _format, D3D12_RESOURCE_FLAGS _flags, D3D12_RESOURCE_STATES _state);
	Texture2D(std::string filepath);
	~Texture2D();
	void LoadFromFile(std::string filepath);
	void Create(int _width, int _height, DXGI_FORMAT _format, void* data);

	// global resource pool, find by filename
	static std::map<std::string, std::unique_ptr<Texture2D>> g_textureResourceMap;
	static Texture2D* Find(std::string filepath);
	static void FreeAll();
};
