#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "GraphicNode.h"

class GraphEditor
{
private:
	std::set<int> usedNodeID;
	std::set<int> usedPinID;

	// pin id : (start, end)
	std::vector<std::pair<int, int>> edges;
	
	std::vector<RenderPassNode> renderPassNodes;
	std::vector<BlitPassNode> blitPassNodes;
	std::vector<TextureNode> textureNodes;
	std::vector<RenderTextureNode> renderTextureNodes;

public:
	GraphEditor();
	~GraphEditor();

	template <typename T_NODE>
	T_NODE RegisterNewNode(std::string nodeName);

	Pin RegisterNewPin(std::string pinName);

	void Init();
	void RenderUI();
};

