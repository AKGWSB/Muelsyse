#pragma once

#include "Timer.h"

class Engine
{
private:
	Timer m_timer;

public:
	Engine();
	~Engine();

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	void Tick(double delta_time);
};
