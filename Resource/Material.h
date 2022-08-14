#pragma once

#include <memory>
#include <unordered_map>

#include "Shader.h"
#include "UploadBuffer.h"
#include "Texture2D.h"

#include "../Library/DirectXTK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class Shader;

class Material
{
private:

public:
	Shader* m_shader;
	std::unordered_map<std::string, Texture2D*> m_textureMap;

	Material();
	~Material();

	void Activate(ID3D12GraphicsCommandList* cmdList);

	void SetShader(Shader* shader);
	void SetTexture(std::string textureName, Texture2D* src);	// set to material "instance"
	void SetMatrix(std::string bufferName, std::string varName, Matrix src);
	void SetFloat4(std::string bufferName, std::string varName, Vector4 src);
};


