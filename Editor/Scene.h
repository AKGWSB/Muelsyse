#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "Actor.h"
#include "../Rendering/Camera.h"
#include "../Rendering/RenderPass.h"
#include "../Rendering/RenderTexture.h"
#include "../Rendering/DepthTexture.h"

class Scene
{
public:
	std::string name;
	std::vector<std::unique_ptr<Actor>> actors;

	// resource pool
	std::map<std::string, Camera> cameraPool;

	Scene();
	~Scene();

	std::vector<Actor*> GetRenderObjects();

	// load scene
	void LoadFromFile(std::string filepath);

	// save scene
	void SaveToFile(std::string filepath);
};
