#include "stdafx.h"
#include "Win32App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Engine e;
    return Win32App::Run(&e, hInstance, nCmdShow);
}
