#include "Scene.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "../Library/json11/json11.hpp"

using namespace std;
using namespace json11;

Scene::Scene()
{

}

Scene::~Scene()
{

}

std::vector<Actor*> Scene::GetRenderObjects()
{
	std::vector<Actor*> res;
	for (auto& up : actors)
	{
		res.push_back(up.get());
	}
	return res;
}

// load scene
void Scene::LoadFromFile(std::string filepath)
{
	// 以读模式打开文件
	std::ifstream in(filepath);
	std::ostringstream tmp;
	tmp << in.rdbuf();

	std::string json_data = tmp.str();
	std::string err_msg;

	// load
	Json json_obj = Json::parse(json_data, err_msg);

	auto j_actors = json_obj["actors"].array_items();
	for (auto& actor : j_actors)
	{
		auto name = actor["name"].string_value();
		auto mesh_path = actor["mesh"].string_value();
		auto material_path = actor["material"].string_value();

		auto pos = actor["position"].array_items();
		XMFLOAT3 position = XMFLOAT3(pos[0].number_value(), pos[1].number_value(), pos[2].number_value());

		auto rot = actor["rotation"].array_items();
		XMFLOAT3 rotation = XMFLOAT3(rot[0].number_value(), rot[1].number_value(), rot[2].number_value());

		auto sca = actor["scale"].array_items();
		XMFLOAT3 scale = XMFLOAT3(sca[0].number_value(), sca[1].number_value(), sca[2].number_value());

		// config and add actor to scene
		actors.push_back(std::make_unique<Actor>());
		auto pActor = actors.back().get();
		pActor->name = name;
		pActor->mesh = Mesh::Find(mesh_path);
		pActor->material = Material::Find(material_path);
		pActor->transform.position = position;
		pActor->transform.rotation = rotation;
		pActor->transform.scale = scale;
	}

	// 关闭打开的文件
	in.close();
}

void SaveToFile(std::string filepath)
{

}
