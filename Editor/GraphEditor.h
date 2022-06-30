#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

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
};

// ----------------------------------------------------------------------- //

class GraphEditor
{
private:
	// find by runtimeID
	static std::map<int, Node> nodePool;
	static std::map<int, Pin> pinPool;

	std::vector<Link> links;

public:
	GraphEditor();
	~GraphEditor();

	static int RegisterNewNode(const Node& node);
	static int RegisterNewPin(const Pin& pin);
	static Node& GetNodeByID(int runtimeID);
	static Pin& GetPinByID(int runtimeID);

	void Init();
	void RenderUI();
	void SaveToFile(std::string filepath);
	void LoadFromFile(std::string filepath);
};

