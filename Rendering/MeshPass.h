#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../Core/CommandListHandle.h"

#include "../Resource/PsoCache.h"
#include "../Resource/Texture2D.h"
#include "../Resource/UploadBuffer.h"

#include "Renderer.h"
#include "Camera.h"
#include "RenderQueue.h"

class MeshPass
{
private:
	std::unique_ptr<UploadBuffer> m_cbPrePass;

public:
	UINT NumRenderTargets = 1;
	DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	EBlendMode queueType = EBlendMode::Opaque;

	std::unordered_map<std::string, Texture2D*> texturesInputPrePass;
	std::vector<RenderTexture*> renderTargets;
	DepthTexture* depthTex;

	MeshPass();
	~MeshPass();

	void Forward(CommandListHandle* cmd, Camera* camera);
};

