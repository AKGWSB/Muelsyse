#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "../Core/d3dx12.h"
#include "../Rendering/DepthTexture.h"

using Microsoft::WRL::ComPtr;

class Editor
{
public:
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	DepthTexture* depthTex;
	bool show_depth_tex = true;
	bool show_demo_window = false;
	bool show_another_window = false;

	Editor();
	~Editor();
	void Init(HWND hwnd);

	void PreGUI();			// call before render GUI
	void PostGUI();			// call after render GUI
	void RenderGUI();		// ..zzz

};
