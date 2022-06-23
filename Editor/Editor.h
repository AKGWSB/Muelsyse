#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "../Core/d3dx12.h"
#include "../Rendering/DepthTexture.h"
#include "../Rendering/RenderPass.h"

#include "Scene.h"
#include "ResourceViewer.h"
#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;

class Editor
{
public:
	int g_width;
	int g_height;
	int actor_item_current_idx = 0;	// current select actor

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	ImFont* font_default;
	ImFont* font_small;

	Scene* scene;	// pass from engine
	Actor* currencSelectedActor = NULL;

	std::unique_ptr<RenderPass> emptyRenderPass;	// set render target to screen use a empty "RenderPass" class
	Texture2D* RT_final;
	Texture2D* depthTex;

	std::unique_ptr<ResourceViewer> resourceViewer;
	std::unique_ptr<GraphEditor> graphicEditor;

	Editor();
	~Editor();
	void Init(int w, int h, HWND hwnd);

	void PreGUI();			// call before render GUI
	void PostGUI();			// call after render GUI
	void RenderGUI();		// ..zzz

};
