#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <map>

#include "../Resource/UploadBuffer.h"
#include "../Resource/Shader.h"
#include "RenderTexture.h"
#include "DepthTexture.h"

using Microsoft::WRL::ComPtr;

class RenderPass
{
public:
	std::unique_ptr<UploadBuffer> cbufferPrePass;

	// shader name --> pso
	std::map<std::string, ComPtr<ID3D12PipelineState>> pipelineStatePool;

	std::string name;
	std::map<std::string, Texture2D*> sourceTextures;	// input
	std::vector<RenderTexture*> renderTargets;			// output
	DepthTexture* depthTex;
	
	RenderPass();
	~RenderPass();

	void InitPipelineState(Shader* shader);
	ID3D12PipelineState* GetPsoByShader(Shader* shader);
};
