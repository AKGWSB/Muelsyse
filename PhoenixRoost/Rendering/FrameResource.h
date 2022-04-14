#pragma once

#include "../stdafx.h"

class FrameResource
{
public:
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;

	UINT64 m_fenceValue;
};