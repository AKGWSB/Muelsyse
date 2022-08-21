#include "RenderQueue.h"

RenderQueue::RenderQueue()
{

}

RenderQueue::~RenderQueue()
{

}

RenderQueue* RenderQueue::GetInstance()
{
	static RenderQueue instance;
	return &instance;
}

void RenderQueue::Submit(Renderer* renderer)
{
	EBlendMode queueType = renderer->material->GetBlendMode();

	switch (queueType)
	{
	case Opaque:
		m_queueOpaque.push_back(renderer);
		break;
	case AlphaTest:
		m_queueAlphaTest.push_back(renderer);
		break;
	case Transparent:
		m_queueTransparent.push_back(renderer);
		break;
	default:
		break;
	}
}

void RenderQueue::ClearAll()
{
	m_queueOpaque.clear();
	m_queueAlphaTest.clear();
	m_queueTransparent.clear();
}

std::vector<Renderer*> RenderQueue::FetchAllRenderers(EBlendMode queueType)
{
	std::vector<Renderer*> res;

	switch (queueType)
	{
	case Opaque:
		res = m_queueOpaque;
		m_queueOpaque.clear();
		break;
	case AlphaTest:
		res = m_queueAlphaTest;
		m_queueAlphaTest.clear();
		break;
	case Transparent:
		res = m_queueTransparent;
		m_queueTransparent.clear();
		break;
	default:
		break;
	}
	return res;
}
