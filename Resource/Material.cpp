#include "Material.h"

Material::Material()
{
	m_cbufferPreMaterial = std::make_unique<UploadBuffer>();
}

Material::~Material()
{

}

void Material::Activate(ID3D12GraphicsCommandList* cmdList)
{
	// set cbuffer for current material
	m_shader->SetCbuffer("cbPreMaterial", m_cbufferPreMaterial.get());

	// set textures
	for (auto& p : m_textureMap)
	{
		m_shader->SetTexture(p.first, p.second);
	}

	m_shader->Activate(cmdList);
}

void Material::SetShader(Shader* shader)
{
	m_shader = shader;

	m_textureMap.clear();
	for (auto& p : m_shader->m_textureBindInfoMap)
	{
		std::string varName = p.first;
		m_textureMap[varName] = NULL;
	}
}

void Material::SetTexture(std::string textureName, Texture2D* src)
{
	if (m_textureMap.find(textureName) == m_textureMap.end())
	{
		return;
	}
	m_textureMap[textureName] = src;
}

void Material::SetCbuffer(std::string bufferName, UploadBuffer* src)
{
	m_shader->SetCbuffer(bufferName, src);
}

void Material::SetMatrix(std::string bufferName, std::string varName, Matrix src)
{
	m_shader->SetMatrix(bufferName, varName, src);
}

void Material::SetFloat4(std::string bufferName, std::string varName, Vector4 src)
{
	m_shader->SetFloat4(bufferName, varName, src);
}
