#pragma once

#include <string>
#include <unordered_map>

#include "../Rendering/Transform.h"
#include "MetaComponent.h"

class MetaComponent;

class Actor
{
private:
	std::unordered_map<std::string, MetaComponent*> m_components;

public:
	Transform transform;

	Actor();
	~Actor();

	MetaComponent* GetComponent(std::string compName);
	void RegisterComponent(std::string compName, MetaComponent* comp);
	void RemoveComponent(std::string compName);

	void OnStart();
	void OnTick(double delta_time);
	void OnRender(ID3D12GraphicsCommandList* cmdList);
	void OnDestroy();
};

