#pragma once

#include "Win32App.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "DX12/d3dx12.h"
#include "DX12/DirectXHelper.h"

#include "DX12/DescriptorHeap.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Engine
{
public:

	// Pipeline objects.
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;

	// desc
	std::shared_ptr<DescriptorHeap> m_rtvHeap;

	// App resources.
	ComPtr<ID3D12Resource> m_uploadBuffer;
	ComPtr<ID3D12Resource> m_defaultBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipelineState;

	// frame resource
	static const UINT FrameCount = 3;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValues[FrameCount];
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[FrameCount];

	Engine();
	~Engine();

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = true);

	void MoveToNextFrame();
	void WaitForGpu();
};
