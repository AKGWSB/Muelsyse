#pragma once


#include <map>
#include <set>
#include <string>
#include <vector>

#include "../Library/json11/json11.hpp"
#include "../Library/imgui/imgui.h"

using namespace json11;

// ----------------------------------------------------------------------- //

enum PinType
{
	TextureRead,
	TextureWrite,
	RenderTargetOutput
};

class Pin
{
public:
	// load from file
	PinType type;
	std::string name;

	// Pin's id, register while runtime
	int runtimeID;

	Pin();
	Pin(PinType t, std::string n);

	Json to_json() const;
};

// ----------------------------------------------------------------------- //

enum NodeType
{
	RenderPassNode,
	BlitPassNode,
	Texture2DNode,
	RenderTextureNode,
	ScreenTargetNode
};

class Node
{
public:
	NodeType type;
	ImVec2 position;

	// the resource held by node
	// maybe a Texture2D, RenderTexture or ...
	std::string name;
	std::string resourceName;

	// list of io pins
	// storage pin's runtimeID
	std::vector<int> inputPins;		// find pin by Link.srcPin
	std::vector<int> outputPins;	// find pin by Link.dstPin

	// node's id, register while runtime
	int runtimeID;

	Node();

	void RenderPins();

	// render depends on node's type
	void Render();

	Json to_json() const;
};

// ----------------------------------------------------------------------- //

// a link from source node's output pin to destination node's input pin
class Link
{
public:
	int srcNode;
	int dstNode;
	int srcPin;
	int dstPin;

	// link's id, register while runtime
	int runtimeID;

	Link();
	Json to_json() const;
};
