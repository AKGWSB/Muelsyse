#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

enum PinType
{
	TextureOutput,
	RenderTextureInput,
	RenderTextureOutput,
	PassInput,
	PassOutput
};

class Pin
{
public:
	std::string name;
	int id;
};

class Node
{
public:
	Node();
	~Node();
	std::string name;
	int id;
	std::vector<Pin> inputPins;
	std::vector<Pin> outputPins;
	void RenderPins();
	virtual void Render();
};

class TextureNode : public Node
{
public:
	TextureNode();
	~TextureNode();
	std::string resourceName;
	void Render();
};

class RenderTextureNode : public Node
{
public:
	RenderTextureNode();
	~RenderTextureNode();
	std::string resourceName;
	void Render();
};

// render all geometries
class RenderPassNode : public Node
{
public:
	RenderPassNode();
	~RenderPassNode();
	void Render();
};

// just blit src tex to dst tex using quad mesh
class BlitPassNode : public Node
{
public:
	BlitPassNode();
	~BlitPassNode();
	void Render();
};