#pragma once

#include <string>
#include <d3d12.h>
#include <wrl.h>
#include <map>

#include "Texture2D.h"
#include "UploadBuffer.h"

using Microsoft::WRL::ComPtr;

struct TextureBindDesc
{
	Texture2D* texture = NULL;
	int bindRegister;
	int bindRegisterSpace;
	int rootParameterIndex;		// index in root parameter, -1 represent resource not found in shader
};

struct CbufferVariableDesc
{
	BYTE data[64];
	int offset;
	int size;
};

struct CbufferBindDesc
{
	UploadBuffer* cbuffer = NULL;
	std::map<std::string, CbufferVariableDesc> variables;
	int bindRegister;
	int bindRegisterSpace;
	int rootParameterIndex;		// index in root parameter, -1 represent resource not found in shader
};

class Shader
{
public:
	std::string name;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3DBlob> vertexShaderByteCode;
	ComPtr<ID3DBlob> pixelShaderByteCode;
	
	// binding infomation from shader code reflection
	std::map<std::string, TextureBindDesc> textureBindInfo;
	std::map<std::string, CbufferBindDesc> cbufferBindInfo;

	Shader(std::string filepath);
	~Shader();
	void LoadFromFile(std::string filepath);
	void RecordReflectionInfo();
	void CreateRootSignature();
	void Activate();

	void SetTexture(std::string textureName, Texture2D* src);
	void SetCbuffer(std::string bufferName, UploadBuffer* src);
	void SetMatrix(std::string bufferName, std::string varName, XMMATRIX src);
	void SetFloat4(std::string bufferName, std::string varName, XMFLOAT4 src);

	// global resource pool, find by filename
	static std::map<std::string, std::unique_ptr<Shader>> g_shaderResourceMap;
	static Shader* Find(std::string filepath);
};
