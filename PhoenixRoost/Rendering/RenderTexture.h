#pragma once

#include "../stdafx.h"

class RenderTexture
{
public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;	// cpu handle
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvDescriptorSize;
};

