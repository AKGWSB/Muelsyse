#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "GraphicNode.h"

class GraphEditor
{
private:
	static std::set<int> usedNodeID;
	static std::set<int> usedPinID;

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
	static T_NODE RegisterNewNode(std::string nodeName);

	static Pin RegisterNewPin(std::string pinName);

	void Init();
	void RenderUI();
};

