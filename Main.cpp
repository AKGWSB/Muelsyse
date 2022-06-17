// for vs setting
/*
d3d12.lib
dxgi.lib
d3dcompiler.lib
dxguid.lib
*/
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxguid.lib")

#include "Core/Win32App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Engine e;
    return Win32App::Run(&e, hInstance, nCmdShow);
}
