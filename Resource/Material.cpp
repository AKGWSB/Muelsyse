#include "Material.h"
#include "../Library/json11/json11.hpp"
#include "../Resource/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace json11;

Material::Material()
{
    
}

Material::Material(std::string filepath)
{
	// 以读模式打开文件
	std::ifstream in(filepath);
	std::ostringstream tmp;
	tmp << in.rdbuf();

	std::string json_data = tmp.str();
	std::string err_msg;

	// load
	Json json_obj = Json::parse(json_data, err_msg);

	// set shader
	auto shader_path = json_obj["shader"].string_value();
	shader = Shader::Find(shader_path);

	// set textures
	auto textures = json_obj["textures"].array_items();
	for (auto& texDesc : textures)
	{
		auto texture_name = texDesc["name"].string_value();
		auto texture_path = texDesc["path"].string_value();
		SetTexture(texture_name, Texture2D::Find(texture_path));
	}

	// 关闭打开的文件
	in.close();
}

Material::~Material()
{

}

void Material::SetTexture(std::string name, Texture2D* tex)
{
    textures[name] = tex;
}

// global resource pool, find by filename
std::map<std::string, std::unique_ptr<Material>> Material::g_materialResourceMap;
Material* Material::Find(std::string filepath)
{
    // from file
    if (g_materialResourceMap.find(filepath) == g_materialResourceMap.end())
    {
        g_materialResourceMap[filepath] = std::make_unique<Material>(filepath);
    }
    return g_materialResourceMap[filepath].get();
}