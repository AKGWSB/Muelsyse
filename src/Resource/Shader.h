#pragma once

#include "../stdafx.h"
#include <string>

class Shader
{
public:
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;

	Shader(std::wstring fullPath);
	~Shader();

	ID3DBlob* GetVertexShader();
	ID3DBlob* GetPixelShader();
};
