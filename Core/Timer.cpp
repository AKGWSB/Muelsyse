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
	QueryPerformanceFrequency(&nf); // ��ȡ������Ƶ��
	QueryPerformanceCounter(&n1);	// ��ȡ��ʼʱ��
}

double Timer::End()
{
	if (!isRunning) return 0.0f;
	isRunning = false;

	QueryPerformanceCounter(&n2); // ��ȡ����ʱ��

	double delta_time = static_cast<double>(static_cast<double>(n2.QuadPart - n1.QuadPart) / static_cast<double>(nf.QuadPart));
	return delta_time;
}

