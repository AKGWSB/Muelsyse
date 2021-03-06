#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <string>
#include "Engine.h"

class Engine;

class Win32App
{
public:
	static int Run(Engine* pEngine, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hwnd; }

	static int m_width;
	static int m_height;
	static std::wstring m_title;
	static HWND m_hwnd;

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	
};

