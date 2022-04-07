#pragma once

#include "stdafx.h"

class Editor;

class Win32App
{
public:
	static int Run(Editor* pEditor, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hwnd; }

	static int m_width;
	static int m_height;
	static std::wstring m_title;
	static HWND m_hwnd;

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	
};

