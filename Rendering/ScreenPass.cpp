#include "ScreenPass.h"

ScreenPass::ScreenPass()
{
	m_cbPrePass = std::make_unique<UploadBuffer>();
	m_quad = std::make_unique<Mesh>();
	m_material = std::make_unique<Material>();
	m_renderer = std::make_unique<Renderer>();

	m_renderer->mesh = m_quad.get();
	m_renderer->material = m_material.get();
}

ScreenPass::~ScreenPass()
{

}

void ScreenPass::Forward(CommandListHandle* cmd)
{
	// build pso for current pass
	PsoDescriptor psoDescPrePass;
	psoDescPrePass.NumRenderTargets = NumRenderTargets;
	for (int i = 0; i < NumRenderTargets; i++)
	{
		psoDescPrePass.RTVFormats[i] = RTVFormats[i];
	}
	psoDescPrePass.DSVFormat = DSVFormat;

	// set rt
	int w = renderTargets[0]->width;
	int h = renderTargets[0]->height;

	cmd->SetRenderTarget(renderTargets, depthTex);
	cmd->ClearRenderTarget(renderTargets, Vector3(0.5, 0.5, 0.5));
	cmd->SetViewPort(Vector4(0, 0, w, h));

	// draw quad
	m_material->SetShader(shader);
	m_renderer->material->SetCbuffer("cbPrePass", m_cbPrePass.get());

	// pass textures
	for (auto& p : texturesInputPrePass)
	{
		std::string varName = p.first;
		Texture2D* tex = p.second;
		m_renderer->material->SetTexture(varName, tex);
	}

	cmd->DrawRenderer(m_renderer.get(), psoDescPrePass);
}
