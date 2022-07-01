#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "GraphNode.h"
#include "../Library/json11/json11.hpp"
#include "../Library/imgui/imgui.h"

class Node;
class Pin;
class Link;

using namespace json11;

class GraphEditor
{
private:
	// find by runtimeID, which represent "pointer" to Node or Pin
	// using int, not c++ pointer, just convenience for json dump
	static std::map<int, Node> nodePool;
	static std::map<int, Pin> pinPool;
	static std::vector<Link> links;

	// current open pipeline file's path
	std::string filepath;

	// nodes need to be delete, clear each frame
	// must delete after "RenderUI", cause STL not support erase while travel
	static std::vector<int> deleteNodeList;

	static void RemoveNodeRightNow(int node_id);

public:
	GraphEditor();
	~GraphEditor();

	static int RegisterNewNode(const Node& node);
	static int RegisterNewPin(const Pin& pin);
	static Node& GetNodeByID(int runtimeID);
	static Pin& GetPinByID(int runtimeID);

	static void AddNode(NodeType type);

	// delete operation will delay until UI render finish
	static void RemoveNode(int node_id);

	void RenderUI();
	void SaveToFile();
	void LoadFromFile(std::string filepath);

	// "compile" the pipeline and generate render pass
	void Execute();

	Json to_json() const;
};

