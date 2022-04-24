#pragma once

#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "../DX12/d3dx12.h"
#include "../DX12/DirectXHelper.h"

#include "DefaultBuffer.h"
#include "../DX12/DescriptorHeap.h"

#include "PathUtil.h"
#include <string>

class Shader
{
public:
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;

	Shader(std::string fullPath);
	~Shader();

	ID3DBlob* GetVertexShader();
	ID3DBlob* GetPixelShader();
};
