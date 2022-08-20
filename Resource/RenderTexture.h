#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "../Core/DescriptorManager.h"
#include "../Core/GraphicContex.h"

using Microsoft::WRL::ComPtr;

class RenderTexture
{
private:
	Descriptor m_rtvDescriptor;
	Descriptor m_srvDescriptor;
	ComPtr<ID3D12Resource> m_buffer;
	DXGI_FORMAT m_format;

public:
	friend class GraphicContex;

	std::string name;
	int width;
	int height;

	RenderTexture(int w, int h, DXGI_FORMAT fmt);
	~RenderTexture();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle();
	void ChangeToRenderTargetState(ID3D12GraphicsCommandList* cmdList);
	void ChangeToShaderRsourceState(ID3D12GraphicsCommandList* cmdList);
};
