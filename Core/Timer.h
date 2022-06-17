#pragma once

#include <iostream>
#include <wrl/client.h>
#include <chrono>

class Timer
{
private:
	LARGE_INTEGER nf, n1, n2;
	bool isRunning = false;
public:
	Timer();
	~Timer();

	void Start();
	double End();
};