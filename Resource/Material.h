#pragma once

#include <string>
#include <map>

#include "Texture2D.h"
#include "Shader.h"

using Microsoft::WRL::ComPtr;

class Material
{
public:

	Shader* shader;

	// varname --> tex
	std::map<std::string, Texture2D*> textures;

	Material();
	~Material();

	void SetTexture(std::string name, Texture2D* tex);
};
