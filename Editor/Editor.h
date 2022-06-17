#pragma once

#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class Editor
{
public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;

	bool show_demo_window = true;
	bool show_another_window = true;

	Editor();
	~Editor();
	void Init(HWND hwnd);

	void PreGUI();			// call before render GUI
	void PostGUI();			// call after render GUI
	void RenderGUI();		// ..zzz

};
