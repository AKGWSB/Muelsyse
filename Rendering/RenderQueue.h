#pragma once

#include <vector>

#include "Renderer.h"

class RenderQueue
{
private:
	std::vector<Renderer*> m_queueOpaque;
	std::vector<Renderer*> m_queueAlphaTest;
	std::vector<Renderer*> m_queueTransparent;

	RenderQueue();

public:
	~RenderQueue();

	// single ton
	static RenderQueue* GetInstance();

	void Submit(Renderer* renderer);
	void ClearAll();

	std::vector<Renderer*> FetchAllRenderers(EBlendMode queueType);
};


