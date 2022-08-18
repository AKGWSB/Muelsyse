#pragma once

#include <d3d12.h>
#include "Actor.h"

class Actor;

class MetaComponent
{
private:
	Actor* m_hostActor;

public:
	MetaComponent() {};
	~MetaComponent() {};

	Actor* GetOwner() { return m_hostActor; };
	virtual void OnStart() {};
	virtual void OnTick(double delta_time) {};
	virtual void OnRender(ID3D12GraphicsCommandList* cmdList) {};
	virtual void OnDestroy() {};
};

