#pragma once

#include <wrl.h>
#include "Timer.h"

class Engine
{
public:
	int g_width;
	int g_height;
	HWND g_hwnd;

	Timer timer;

	Engine();
	~Engine();

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	void Tick(double delta_time);
};
