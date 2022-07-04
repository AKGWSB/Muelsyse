#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "../Core/d3dx12.h"
#include "../Rendering/DepthTexture.h"
#include "../Rendering/RenderPass.h"

#include "Scene.h"
#include "GraphEditor.h"

#include "../Library/imgui/imgui.h"
#include "../Library/imgui/imgui_impl_win32.h"
#include "../Library/imgui/imgui_impl_dx12.h"

using Microsoft::WRL::ComPtr;

class Editor
{
private:
	int g_width;
	int g_height;

	// current select actor
	int actor_item_current_idx = 0;	
	Actor* currencSelectedActor = NULL;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;		// cpu handle for srv descriptor
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;		// gpu handle for srv descriptor
	UINT srvHandleIndex;							// index in srv descriptor heap

	ImFont* font_default;
	ImFont* font_small;

	// frame graph
	std::unique_ptr<GraphEditor> graphicEditor;

	// render imgui to screen
	std::unique_ptr<RenderPass> emptyRenderPass;

	// render for ImGui
	void PreGUI();			// call before render GUI
	void PostGUI();			// call after render GUI
	void RenderGUI();		// ..zzz

public:
	// pass from engine
	Scene* scene;	
	Texture2D* RT_final;
	Texture2D* depthTex;

	Editor();
	~Editor();

	void Init(int w, int h, HWND hwnd);
	void Destory();
	void Render();
};
