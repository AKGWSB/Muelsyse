#pragma once

#include "Texture2D.h"

class RenderTexture : public Texture2D
{
private:
	Descriptor m_rtvDescriptor;

public:
	RenderTexture(int w, int h, DXGI_FORMAT fmt);
	~RenderTexture();

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle();
	void ChangeToRenderTargetState(ID3D12GraphicsCommandList* cmdList);
	void ChangeToShaderRsourceState(ID3D12GraphicsCommandList* cmdList);
};
