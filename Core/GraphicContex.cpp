#include "GraphicContex.h"
#include "helper.h"
#include "d3dx12.h"
#include "Win32App.h"

// ---------------------------------------------------------------------- //

void GraphicContex::Init()
{
    screenWidth = Win32App::m_width;
    screenHeight = Win32App::m_height;

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter, true);
    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));


    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));


    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = m_frameCount;
    swapChainDesc.Width = screenWidth;
    swapChainDesc.Height = screenHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        Win32App::m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(Win32App::m_hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();


    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = m_frameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));


    // Create a RTV for each frame.
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_frameCount; i++)
    {
        // create buffer
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);

        // record rtv handle
        Descriptor rtvDesc;
        rtvDesc.cpuHandle = rtvHandle;
        m_renderTargetViews[i] = rtvDesc;

        rtvHandle.Offset(1, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
    }


    // Create synchronization objects.
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }


    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
}


// ---------------------------------------------------------------------- //

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

// ---------------------------------------------------------------------- //

GraphicContex::GraphicContex()
{
    //OutputDebugStringA("GraphicContex de cons\n");
}

void GraphicContex::WaitForGpu()
{
    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void GraphicContex::Begin()
{
    // set the descriptor heap
    DescriptorManager* descManager = DescriptorManager::GetInstance();
    ID3D12DescriptorHeap* g_srvHeap = descManager->m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetD3DHeap();
    ID3D12DescriptorHeap* g_samplerHeap = descManager->m_descriptorHeapMap[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER]->GetD3DHeap();

    ID3D12DescriptorHeap* ppHeaps[] = { g_srvHeap, g_samplerHeap };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void GraphicContex::End()
{
    // Indicate that the back buffer will now be used to present.
    auto backBuffer = m_renderTargets[m_currentBackBufferIndex].Get();
    auto trans = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &trans);

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandList->Close();
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    
    // swap
    ThrowIfFailed(m_swapChain->Present(1, 0));
    m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // sync
    WaitForGpu();

    // clear
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), NULL));
}

GraphicContex* GraphicContex::GetInstance()
{
    static GraphicContex instance;
    return &instance;
}

ID3D12Device* GraphicContex::GetDevice()
{
    return m_device.Get();
}

ID3D12GraphicsCommandList* GraphicContex::GetCommandList()
{
    return m_commandList.Get();
}

// ---------------------------------------------------------------------- //

void GraphicContex::SyncExecute(ID3D12GraphicsCommandList* cmdList)
{
    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { cmdList };
    ThrowIfFailed(cmdList->Close());
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForGpu();

    // clear
    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), NULL));
}

void GraphicContex::SetRenderTarget()
{
    auto backBuffer = m_renderTargets[m_currentBackBufferIndex].Get();
    auto trans = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &trans);

    auto rtvHandle = m_renderTargetViews[m_currentBackBufferIndex].cpuHandle;
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);
}

void GraphicContex::ClearRenderTarget(Vector3 clearColor)
{
    auto rtvHandle = m_renderTargetViews[m_currentBackBufferIndex].cpuHandle;
    float _clearColor[4] = { clearColor.x, clearColor.y, clearColor.z, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, _clearColor, 0, nullptr);
}

void GraphicContex::SetViewPort(Vector4 rect)
{
    CD3DX12_VIEWPORT m_viewport(rect.x, rect.y, rect.z, rect.w);
    CD3DX12_RECT m_scissorRect(rect.x, rect.y, rect.z, rect.w);
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
}
