#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "../Core/d3dx12.h"
#include "../Rendering/DepthTexture.h"
#include "../Rendering/RenderPass.h"
#include "Scene.h"

using Microsoft::WRL::ComPtr;

class Editor
{
public:
	int g_width;
	int g_height;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	Scene* scene;	// pass from engine

	std::unique_ptr<RenderPass> emptyRenderPass;	// set render target to screen use a empty "RenderPass" class
	Texture2D* RT_final;
	Texture2D* depthTex;
	bool show_depth_tex = true;
	bool show_demo_window = false;
	bool show_another_window = false;

	Editor();
	~Editor();
	void Init(int w, int h, HWND hwnd);

	void PreGUI();			// call before render GUI
	void PostGUI();			// call after render GUI
	void RenderGUI();		// ..zzz

};
