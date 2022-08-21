#include "Renderer.h"

Renderer::Renderer(const Matrix& t, Mesh* m, Material* mat)
{
	transMat = t;
	mesh = m;
	material = mat;

	m_cbPreObject = std::make_unique<UploadBuffer>(256);
}

Renderer::Renderer()
{
	m_cbPreObject = std::make_unique<UploadBuffer>(256);
}

Renderer::~Renderer()
{

}
