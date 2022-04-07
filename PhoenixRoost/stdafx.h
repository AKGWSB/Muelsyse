#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include "DirectXHelper.h"

#include <wrl.h>
#include <shellapi.h>

#include <algorithm>
#include <string>
#include <vector>

#include "Win32App.h"

#include "Editor/Editor.h"

// config it in visual studio's pannel
/*
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
*/
