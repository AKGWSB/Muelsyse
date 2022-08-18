#include "Actor.h"


Actor::Actor()
{

}

Actor::~Actor()
{

}

MetaComponent* Actor::GetComponent(std::string compName)
{
	auto it = m_components.find(compName);
	if (it != m_components.end())
	{
		return it->second;
	}
	return NULL;
}

void Actor::RegisterComponent(std::string compName, MetaComponent* comp)
{
	auto it = m_components.find(compName);
	if (it != m_components.end())
	{
		std::string dbg = "Fail to register comp [" + compName + "]";
		throw std::exception(dbg.c_str());
	}

	m_components[compName] = comp;
}

void Actor::RemoveComponent(std::string compName)
{
	m_components.erase(compName);
}

void Actor::OnStart()
{

}

void Actor::OnTick(double delta_time)
{

}

void Actor::OnRender() {

}

void Actor::OnDestroy()
{

}
