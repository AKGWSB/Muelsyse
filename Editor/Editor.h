#include "../stdafx.h"

#ifndef __EDITOR__
#define __EDITOR__

class Editor
{
public:
	Editor(HINSTANCE hInstance, int ShowWnd);
	~Editor();

private:
	// width and height of the window
	int Width = 800;
	int Height = 600;

	// Handle to the window
	HWND hwnd = NULL;

	// name of the window (not the title)
	LPCTSTR WindowName = L"PhoenixRoost Editor";

	// title of the window
	LPCTSTR WindowTitle = L"PhoenixRoost Editor";

	bool InitializeWindow(HINSTANCE hInstance, int nShowCmd);

	void MainLoop();

	/*
	// deal with windows messages that are sent from Windows
	LRESULT CALLBACK WndProc(HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);*/
};

#endif