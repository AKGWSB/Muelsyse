#pragma once

#include "../stdafx.h"

class Device
{
public:
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;

	static const UINT FrameCount = 2;

	// Pipeline objects.
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;

	Device(HWND hwnd, int width, int height);
	~Device();

	void MoveToNextFrame();
	void WaitForGpu();

	// find adapter
	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = true);

	
};
