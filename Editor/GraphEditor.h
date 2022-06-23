#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

class Pin
{
public:
	std::string name;
	int id;
};

class Node
{
public:
	std::string name;
	int id;
};

class GraphEditor
{
private:
	// node name --> node id
	std::set<int> usedNodeID;

	// attribute name --> attribute id
	std::set<int> usedPinID;

public:
	GraphEditor();
	~GraphEditor();

	int RegisterNewNode(std::string nodeName);
	int RegisterNewAttribute(std::string attrName);

	void RenderUI();
};

