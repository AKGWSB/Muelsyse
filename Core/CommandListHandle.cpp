#include "CommandListHandle.h"

#include "GraphicContex.h"

CommandListHandle::CommandListHandle(ID3D12GraphicsCommandList* cmdList)
{
	m_cmdList = cmdList;
}

CommandListHandle::~CommandListHandle()
{

}

// ---------------------------------------------------------------------- //

/*
void SetRenderTarget(std::vector<RenderTexture*> renderTargets, DepthTexture* depthTex)
{
	// target is screen
	if (renderTargets.size() == 0)
	{
		
	}
}

void ClearRenderTarget(std::vector<RenderTexture*> renderTargets, Vector3 clearColor)
{

}

void ClearDepthBuffer(DepthTexture* depthTex)
{

}
*/
