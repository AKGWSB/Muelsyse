#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>

#include "DescriptorHeap.h"
#include "../Resource/Mesh.h"
#include "../Resource/Shader.h"
#include "../Resource/Texture2D.h"
#include "../Resource/UploadBuffer.h"
#include "../Rendering/RenderTexture.h"
#include "../Rendering/RenderPass.h"
#include "../Rendering/Camera.h"
#include "../Editor/Actor.h"

using Microsoft::WRL::ComPtr;

class DescriptorHeap;
class Buffer;
class Shader;
class Mesh;
class Texture2D;
class UploadBuffer;

class GraphicContex
{
public:
    static int screenWidth;
    static int screenHeight;
    static const UINT FrameCount = 2;

    // Pipeline objects.
    static ComPtr<IDXGISwapChain3> g_swapChain;
    static ComPtr<ID3D12Device> g_device;

    // frame resource
    static UINT m_rtvIndices[FrameCount];
    static ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

    static ComPtr<ID3D12CommandAllocator> g_commandAllocator;
    static ComPtr<ID3D12CommandQueue> g_commandQueue;
    static ComPtr<ID3D12GraphicsCommandList> g_commandList;

    // descriptor heaps
    static std::unique_ptr<DescriptorHeap> g_rtvHeap;   // render target view
    static std::unique_ptr<DescriptorHeap> g_srvHeap;   // shader resource view
    static std::unique_ptr<DescriptorHeap> g_splHeap;   // sampler view
    static std::unique_ptr<DescriptorHeap> g_dsvHeap;   // depth stencil

    // Synchronization objects.
    static UINT m_frameIndex;
    static HANDLE m_fenceEvent;
    static ComPtr<ID3D12Fence> m_fence;
    static UINT64 m_fenceValue;

    static void Init(int width, int height, HWND hwnd);
    static void PreRender();
    static void Render();
    static void PostRender();
    static void Destory();

    static void WaitForPreviousFrame();
    static ID3D12Resource* GetScreenRenderTarget();

    static void SetViewport(int width, int height);
    static void ClearRenderTarget(RenderPass* pass, XMFLOAT3 clearColor);
    static void SetRenderTarget(RenderPass* pass);
    static void RenderLoop(Camera* camera, RenderPass* renderPass, std::vector<Actor*> renderObjects);
};

void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = true);