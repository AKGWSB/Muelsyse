#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <map>

#include "Engine.h"
#include "DescriptorManager.h"

#include "../Resource/Mesh.h"
#include "../Resource/Material.h"
#include "../Resource/RenderTexture.h"

#include "../Library/DirectXTK/SimpleMath.h"

class Engine;
class RenderTexture;

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

class GraphicContex
{
private:
    int screenWidth;
    int screenHeight;
    static const UINT m_frameCount = 2;

    // command
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    // Pipeline objects.
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;

    // Synchronization objects.
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    // frame resource
    UINT m_currentBackBufferIndex;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    std::unique_ptr<RenderTexture> m_renderTargets[m_frameCount];

    void Init();

    GraphicContex();
    GraphicContex(const GraphicContex&);
    GraphicContex& operator=(const GraphicContex&);

    void WaitForGpu();

    // called by Engine as friend class
    void Begin();   // before populate
    void End();     // after populate, swap and present current frame
    void Shutdown();

public:
    friend class Engine;
    friend class CommandListHandle;

    ~GraphicContex() { };

    // single ton
    static GraphicContex* GetInstance();

    ID3D12Device* GetDevice();   
    ID3D12GraphicsCommandList* GetCommandList();

    // exe and wait gpu
    void SyncExecute(ID3D12GraphicsCommandList* cmdList);    

    // return current frams's back RT
    RenderTexture* GetCurrentBackBuffer();
};

void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = true);