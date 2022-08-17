#pragma once

#include <d3d12.h>

class MetaComponent
{
private:

public:
	MetaComponent() {};
	~MetaComponent() {};

	virtual void OnStart() = 0;
	virtual void OnTick(double delta_time) = 0;
	virtual void OnRender(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void OnDestroy() = 0;
};

