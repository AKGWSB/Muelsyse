#include "MeshPass.h"

MeshPass::MeshPass()
{
	m_cbPrePass = std::make_unique<UploadBuffer>();
}

MeshPass::~MeshPass()
{

}

void MeshPass::Forward(CommandListHandle* cmd, Camera* camera)
{
	// find renderers
	auto renderQueue = RenderQueue::GetInstance();
	std::vector<Renderer*> renderers = renderQueue->FetchAllRenderers(queueType);

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
	cmd->ClearDepthBuffer(depthTex);
	cmd->SetViewPort(Vector4(0, 0, w, h));

	// loop draw
	for (auto& renderer : renderers)
	{
		Matrix v = camera->GetViewMatrix();
		Matrix p = camera->GetProjectionMatrix();

		// pass cbuffer
		renderer->material->SetCbuffer("cbPrePass", m_cbPrePass.get());
		renderer->material->SetMatrix("cbPrePass", "viewMatrix", v);
		renderer->material->SetMatrix("cbPrePass", "projectionMatrix", p);
		renderer->material->SetFloat4("cbPrePass", "vec4Debug", Vector4(1, 0, 0, 1));

		// pass textures
		for (auto& p : texturesInputPrePass)
		{
			std::string varName = p.first;
			Texture2D* tex = p.second;
			renderer->material->SetTexture(varName, tex);
		}

		cmd->DrawRenderer(renderer, psoDescPrePass);
	}
}
