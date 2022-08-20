#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "../Core/DescriptorManager.h"
#include "../Core/GraphicContex.h"

using Microsoft::WRL::ComPtr;

class GraphicContex;

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

	/*
	// PIXEL_SHADER_RESOURCE --> RENDER_TARGET
	void ChangeToRenderTargetState(ID3D12GraphicsCommandList* cmdList);

	// RENDER_TARGET --> PIXEL_SHADER_RESOURCE
	void ChangeToShaderRsourceState(ID3D12GraphicsCommandList* cmdList);
	*/

	// Resource Barrier, change from "curStat" to "dstStat"
	void ChangeState(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES curStat, D3D12_RESOURCE_STATES dstStat);
};
