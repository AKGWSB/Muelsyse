#include "stdafx.h"
#include "Win32App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Editor editor;
    return Win32App::Run(&editor, hInstance, nCmdShow);
}
