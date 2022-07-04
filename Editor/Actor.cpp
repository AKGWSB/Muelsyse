#include "Actor.h"

Actor::Actor()
{
	cbufferPreObject = std::make_unique<UploadBuffer>();
}

Actor::~Actor()
{

}

void Actor::Draw()
{
	auto shader = material->shader;

	// set texture to shader
	for (auto& p : material->textures)
	{
		auto texName = p.first;
		auto tex = p.second;
		shader->SetTexture(texName, tex);
	}

	// set model matrix
	shader->SetCbuffer("cbPreObject", cbufferPreObject.get());
	shader->SetMatrix("cbPreObject", "modelMatrix", transform.GetTransformMatrix());

	// active shader
	shader->Activate();

	// draw call
	mesh->Draw();

	shader->DeActivateForDebug();
}
