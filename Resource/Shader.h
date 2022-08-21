#pragma once

#include <string>
#include <d3d12.h>
#include <wrl.h>
#include <map>

#include "UploadBuffer.h"
#include "Texture2D.h"
#include "Material.h"

#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

class Material;

struct TextureBindDesc
{
	Texture2D* texture = NULL;
	int bindRegister;
	int bindRegisterSpace;
	int rootParameterIndex = -1;		// index in root parameter, -1 represent resource not found in shader
};

struct CbufferVariableDesc
{
	int offset;
	int size;
};

struct CbufferBindDesc
{
	UploadBuffer* cbuffer = NULL;
	std::unordered_map<std::string, CbufferVariableDesc> variableDescMap;
	int bindRegister;
	int bindRegisterSpace;

	// index in root parameter, a resource maybe used in both PS and VS
	// so it will has 2 root param with both "D3D12_SHADER_VISIBILITY_VERTEX" and "D3D12_SHADER_VISIBILITY_PIXEL" 
	// both it will has 2 param index
	// see: https://zhuanlan.zhihu.com/p/425391931
	int rootParameterIndexPS = -1;		
	int rootParameterIndexVS = -1;
};

class Shader
{
	friend class Material;

private:
	// binding infomation from shader code reflection
	std::unordered_map<std::string, TextureBindDesc> m_textureBindInfoMap;
	std::unordered_map<std::string, CbufferBindDesc> m_cbufferBindInfoMap;

	void LoadFromFile(std::string filepath);
	void CreateRootSignature();

public:
	std::string name;

	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3DBlob> m_vertexShaderByteCode;
	ComPtr<ID3DBlob> m_pixelShaderByteCode;

	Shader(std::string filepath);
	~Shader();

	void Activate(ID3D12GraphicsCommandList* cmdList);

	void SetTexture(std::string textureName, Texture2D* src);
	void SetCbuffer(std::string bufferName, UploadBuffer* src);
	void SetMatrix(std::string bufferName, std::string varName, Matrix src);
	void SetFloat4(std::string bufferName, std::string varName, Vector4 src);
};
