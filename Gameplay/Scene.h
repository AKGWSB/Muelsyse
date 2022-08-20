#pragma once

#include "Actor.h"

class Scene
{
private:
	std::unordered_map<std::string, Actor*> m_actors;

public:
	Scene();
	~Scene();


};

Scene::Scene()
{
}

Scene::~Scene()
{
}
