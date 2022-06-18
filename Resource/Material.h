#pragma once

#include <string>
#include <map>

#include "Texture2D.h"
#include "Shader.h"

using Microsoft::WRL::ComPtr;

class Material
{
public:
	std::string name;
	Shader* shader;

	// varname --> tex
	std::map<std::string, Texture2D*> textures;

	Material();
	Material(std::string filepath);
	~Material();

	void SetTexture(std::string name, Texture2D* tex);

	// global resource pool, find by filename
	static std::map<std::string, std::unique_ptr<Material>> g_materialResourceMap;
	static Material* Find(std::string filepath);
};
