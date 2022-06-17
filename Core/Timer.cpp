#include "Timer.h"

Timer::Timer()
{

}

Timer::~Timer()
{

}

void Timer::Start()
{
	isRunning = true;
	QueryPerformanceFrequency(&nf); // 获取计数器频率
	QueryPerformanceCounter(&n1);	// 获取开始时间
}

double Timer::End()
{
	if (!isRunning) return 0.0f;
	isRunning = false;

	QueryPerformanceCounter(&n2); // 获取结束时间

	double delta_time = static_cast<double>(static_cast<double>(n2.QuadPart - n1.QuadPart) / static_cast<double>(nf.QuadPart));
	return delta_time;
}

