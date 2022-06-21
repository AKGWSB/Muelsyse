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
	this->filepath = filepath;

	// 以读模式打开文件
	std::ifstream in(filepath);
	std::ostringstream tmp;
	tmp << in.rdbuf();

	std::string json_data = tmp.str();
	std::string err_msg;

	// load
	Json json_obj = Json::parse(json_data, err_msg);

	name = json_obj["name"].string_value();

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

void Scene::SaveToFile()
{
	std::string j_str = "{\"name\":\"" + name + "\",  \"actors\" :  [";

	std::vector<Json> j_actors;

	for (auto& actor : actors)
	{
		// save actor
		Json actor_json_obj = Json::object{
			{ "name", actor->name },
			{ "mesh", actor->mesh->name },
			{ "material", actor->material->name },
			{ "position", Json::array {actor->transform.position.x, actor->transform.position.y, actor->transform.position.z} },
			{ "rotation", Json::array {actor->transform.rotation.x, actor->transform.rotation.y, actor->transform.rotation.z} },
			{ "scale", Json::array {actor->transform.scale.x, actor->transform.scale.y, actor->transform.scale.z} },
		};

		std::string j_a;
		actor_json_obj.dump(j_a);
		j_str += j_a + ",";

		// save material
		std::string mj_str = "{";
		mj_str += "\"shader\":\"" + actor->material->shader->name +"\",";
		mj_str += "\"textures\":[";
		for (auto& tp : actor->material->textures)
		{
			auto& tex_varname = tp.first;
			auto& tex_filepath = tp.second->name;
			mj_str += "{\"name\":\"" + tex_varname + "\",\"path\":\"" + tex_filepath + "\"},";
		}
		mj_str.erase(mj_str.end() - 1);	// remove last ','
		mj_str += "]}";

		// write
		std::ofstream out(actor->material->name);
		out << mj_str;
		out.close();
	}

	j_str.erase(j_str.end() - 1);	// remove last ','

	j_str += "]}";


	// write
	std::ofstream out(filepath);
	out << j_str;
	out.close();
}
