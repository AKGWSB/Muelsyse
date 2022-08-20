#pragma once

#include <d3d12.h>
#include <vector>

//#include "../Resource/RenderTexture.h"
//#include "../Resource/DepthTexture.h"

//#include "../Library/DirectXTK/SimpleMath.h"

//using namespace DirectX::SimpleMath;

class CommandListHandle
{
private:
	ID3D12GraphicsCommandList* m_cmdList;

public:
	CommandListHandle(ID3D12GraphicsCommandList* cmdList);
	~CommandListHandle();

	/*
	// pass "renderTargets = {}" is for screen buffer
	void SetRenderTarget(std::vector<RenderTexture*> renderTargets, DepthTexture* depthTex);
	void ClearRenderTarget(std::vector<RenderTexture*> renderTargets, Vector3 clearColor);
	void ClearDepthBuffer(DepthTexture* depthTex);
	*/

};

